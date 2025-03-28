/* Firmware loader for Samsung TV cameras */
/* Copyright (c) 2025 Ondrej Zary (excluding firmware files)*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <libusb-1.0/libusb.h>
#include <getopt.h>
#include <fcntl.h>
#include <errno.h>

#define TIMEOUT		1000
#define DIR_IN		0
#define DIR_OUT		1

#define AIT_IDX		0x600

#define AIT_CMD_UPLOAD	0x03
#define AIT_CMD_FW	0x04

#define AIT_FW_CHUNK_SIZE	32

#define FW_DIR		"/lib/firmware/samsung-tvcam/"

enum cam_type { CAM_AIT, CAM_MAX };

struct camera {
	char *name;
	uint16_t vendor;
	uint16_t product;
	enum cam_type type;
	char *firmware;
};

static const struct camera cameras[] = {
	{ .name = "VG-STC2000", .vendor = 0x04e8, .product = 0x2059, .type = CAM_MAX, .firmware = "raptor_firmware.img" },
	{ .name = "VG-STC3000", .vendor = 0x04e8, .product = 0x205c, .type = CAM_AIT, .firmware = "FalconFW.bin" },
	{ .name = "VG-STC4000", .vendor = 0x04e8, .product = 0x2061, .type = CAM_AIT, .firmware = "FalconPlus_FW.bin" },
	{ .name = "VG-STC5000", .vendor = 0x04e8, .product = 0x2065, .type = CAM_AIT, .firmware = "Heron_Ext_FW.bin" },
	{ }
};

static int ait_cmd(libusb_device_handle *handle, uint8_t *data, uint16_t val, uint16_t len, uint8_t out) {
	int req = 1;
	int req_type = LIBUSB_REQUEST_TYPE_CLASS;

	if (!out) {
		req |= LIBUSB_ENDPOINT_IN;
		req_type |= LIBUSB_ENDPOINT_IN;
	}

	return libusb_control_transfer(handle, req_type, req, val << 8, AIT_IDX, data, len, TIMEOUT);
}

static bool ait_upload_fw_uvc(libusb_device_handle *handle, FILE *f) {
	/* start firmware upload */
	uint8_t cmd[8] = { 0x01, 0x00, 0x03 };
	int cnt = ait_cmd(handle, cmd, AIT_CMD_FW, sizeof(cmd), DIR_OUT);
	if (cnt < 0) {
		fprintf(stderr, "Error sending FW load command: %s\n", libusb_strerror(cnt));
		return false;
	}
	/* upload firmware in chunks */
	while (!feof(f)) {
		uint8_t buf[AIT_FW_CHUNK_SIZE];
		memset(buf, 0, sizeof(buf));
		cnt = fread(buf, 1, AIT_FW_CHUNK_SIZE, f);
		if (cnt < AIT_FW_CHUNK_SIZE && ferror(f)) {
			fprintf(stderr, "Error reading firmware file: %s\n", strerror(errno));
			return false;
		}
		cnt = ait_cmd(handle, buf, AIT_CMD_UPLOAD, AIT_FW_CHUNK_SIZE, DIR_OUT);
		if (cnt < 0) {
			fprintf(stderr, "Error uploading FW: %s\n", libusb_strerror(cnt));
			return false;
		}
	}
	/* boot uploaded firmware */
	cmd[1] = 0x01;
	cnt = ait_cmd(handle, cmd, AIT_CMD_FW, sizeof(cmd), DIR_OUT);
	if (cnt < 0) {
		fprintf(stderr, "Error sending FW boot command: %s\n", libusb_strerror(cnt));
		return false;
	}

	return true;
}

#define MAX_REQ_FW	0xde

#define MAX_FW_CHUNK_SIZE	4088

static bool max_upload_fw(libusb_device_handle *handle, FILE *f) {
	uint8_t req_type = LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_INTERFACE;

	/* start firmware upload */
	if (libusb_control_transfer(handle, req_type, MAX_REQ_FW - 1, 0, 0, 0, 0, 0) < 0)
		return false;

	/* upload firmware in chunks */
	int i = 0;
	while (!feof(f)) {
		uint8_t buf[MAX_FW_CHUNK_SIZE];
		memset(buf, 0, sizeof(buf));
		int cnt = fread(buf, 1, MAX_FW_CHUNK_SIZE, f);
		if (cnt < MAX_FW_CHUNK_SIZE && ferror(f)) {
			fprintf(stderr, "Error reading firmware file: %s\n", strerror(errno));
			return false;
		}
		if (libusb_control_transfer(handle, req_type, MAX_REQ_FW, i >> 16, i & 0xffff, buf, MAX_FW_CHUNK_SIZE, 0) != MAX_FW_CHUNK_SIZE) {
			fprintf(stderr, "Error uploading FW: %s\n", libusb_strerror(cnt));
			return false;
		}
		i++;
	}

	return true;
}

static bool upload_fw(libusb_device_handle *handle, const char *filename, enum cam_type type) {
	/* chdir to FW_DIR only if filename is not absolute or relative to current directory */
	if (!filename || (filename[0] != '/' && filename[0] != '.' && chdir(FW_DIR))) {
		fprintf(stderr, "Error accessing firmware directory %s: %s\n", FW_DIR, strerror(errno));
		return false;
	}

	FILE *f = fopen(filename, "r");
	if (!f) {
		fprintf(stderr, "Error opening firmware file %s%s: %s\n", (filename[0] != '/' && filename[0] != '.') ? FW_DIR : "" , filename, strerror(errno));
		return false;
	}

	bool ret;
	if (type == CAM_MAX)
		ret = max_upload_fw(handle, f);
	else
		ret = ait_upload_fw_uvc(handle, f);

	fclose(f);

	return ret;
}

static const struct camera *find_camera(uint16_t vendor, uint16_t product) {
	const struct camera *cam = NULL;

	for (int i = 0; cameras[i].name; i++)
		if (cameras[i].vendor == vendor && cameras[i].product == product)
			cam = &cameras[i];

	return cam;
}

static int usage(char *argv[]) {
	fprintf(stderr, "Usage: \n");
	fprintf(stderr, " %s -a\n", argv[0]);
	fprintf(stderr, " %s -d device_path -t type -f firmware.bin\n", argv[0]);
	fprintf(stderr, "\nLoad firmware to Samsung VG-STC2000, VG-STC3000, VG-STC4000 or VG-STC5000 TV camera.\n");
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "  -a\tautomatically load firmware to all supported Samsung TV cameras\n");
	fprintf(stderr, "  -d\tUSB device path (/dev/bus/usb/xxx/yyy)\n");
	fprintf(stderr, "  -t\tcamera type (max or ait)\n");
	fprintf(stderr, "  -f\tfirmware file to load (relative to %s if not starting with '/' or '.')\n", FW_DIR);
	return EXIT_FAILURE;
}

int main(int argc, char *argv[]) {
	bool all = false;
	char *device_path = NULL, *firmware_path = NULL, *cam_type = NULL;
	int opt;

	while ((opt = getopt(argc, argv, "ad:f:t:")) != -1) {
		switch (opt) {
		case 'a':
			all = true;
			break;
		case 'd':
			device_path = optarg;
			break;
		case 'f':
			firmware_path = optarg;
			break;
		case 't':
			cam_type = optarg;
			break;
		default:
			return usage(argv);
		}
	}

	libusb_device_handle *handle;

	if (all) {
		if (device_path || firmware_path || cam_type) {
			fprintf(stderr, "-a must be used alone!\n");
			return usage(argv);
		}

		int err = libusb_init(NULL);
		if (err) {
			fprintf(stderr, "Error initializing libusb: %s\n", libusb_strerror(err));
			return EXIT_FAILURE;
		}

		libusb_device **list;
		ssize_t cnt = libusb_get_device_list(NULL, &list);
		if (cnt < 0) {
			fprintf(stderr, "Error getting USB device list: %s\n", libusb_strerror(cnt));
			libusb_exit(NULL);
			return EXIT_FAILURE;
		}

		int found = 0;
		for (ssize_t i = 0; i < cnt; i++) {
			struct libusb_device_descriptor desc;
			err = libusb_get_device_descriptor(list[i], &desc);
			if (err) {
				fprintf(stderr, "Error getting USB device descriptor: %s\n", libusb_strerror(err));
				continue;
			}
			const struct camera *cam = find_camera(desc.idVendor, desc.idProduct);
			if (cam) {
				found++;
				printf("Found camera %s: uploading firmware %s\n", cam->name, cam->firmware);
				err = libusb_open(list[i], &handle);
				if (err) {
					fprintf(stderr, "Error opening USB device: %s\n", libusb_strerror(err));
					continue;
				}

				if (!upload_fw(handle, cam->firmware, cam->type))
					fprintf(stderr, "Firmware upload failed!\n");

				libusb_close(handle);
			}
		}
		if (found < 1)
			fprintf(stderr, "No supported cameras found.\n");
		libusb_free_device_list(list, 1);
	} else {
		if (!device_path || !firmware_path || !cam_type) {
			fprintf(stderr, "Both device and firmware path and also device type must be specified!\n");
			return usage(argv);
		}
		if (strcmp(cam_type, "ait") && strcmp(cam_type, "max")) {
			fprintf(stderr, "Invalid camera type '%s'. Must be either 'ait' or 'max'\n", cam_type);
			return usage(argv);
		}

		libusb_set_option(NULL, LIBUSB_OPTION_NO_DEVICE_DISCOVERY);
		int err = libusb_init(NULL);
		if (err) {
			fprintf(stderr, "Error initializing libusb: %s\n", libusb_strerror(err));
			return EXIT_FAILURE;
		}

		int fd = open(device_path, O_RDWR);
		if (fd < 0) {
			fprintf(stderr, "Error opening %s: %s\n", device_path, strerror(errno));
			return EXIT_FAILURE;
		}

		err = libusb_wrap_sys_device(NULL, fd, &handle);
		if (err) {
			fprintf(stderr, "Error wrapping device %s: %s\n", device_path, libusb_strerror(err));
			close(fd);
			return EXIT_FAILURE;
		}

		if (!upload_fw(handle, firmware_path, !strcmp(cam_type, "max") ? CAM_MAX : CAM_AIT))
			fprintf(stderr, "Firmware upload failed\n");

		libusb_close(handle);
		close(fd);
	}

	libusb_exit(NULL);
	return 0;
}
