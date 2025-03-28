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

#define IDX		0x600

#define CMD_UPLOAD	0x03
#define CMD_FW		0x04

#define FW_CHUNK_SIZE	32

#define FW_DIR		"/lib/firmware/samsung-tvcam/"

struct camera {
	char *name;
	uint16_t vendor;
	uint16_t product;
	char *firmware;
};

static const struct camera cameras[] = {
	{ .name = "VG-STC3000", .vendor = 0x04e8, .product = 0x205c, .firmware = "FalconFW.bin" },
	{ .name = "VG-STC4000", .vendor = 0x04e8, .product = 0x2061, .firmware = "FalconPlus_FW.bin" },
	{ .name = "VG-STC5000", .vendor = 0x04e8, .product = 0x2065, .firmware = "Heron_Ext_FW.bin" },
	{ }
};

static int ait_cmd(libusb_device_handle *handle, uint8_t *data, uint16_t val, uint16_t len, uint8_t out) {
	int req = 1;
	int req_type = LIBUSB_REQUEST_TYPE_CLASS;

	if (!out) {
		req |= LIBUSB_ENDPOINT_IN;
		req_type |= LIBUSB_ENDPOINT_IN;
	}

	return libusb_control_transfer(handle, req_type, req, val << 8, IDX, data, len, TIMEOUT);
}

static bool ait_upload_fw_uvc(libusb_device_handle *handle, char *filename) {
	int ret = false;

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
	/* start firmware upload */
	uint8_t cmd[8] = { 0x01, 0x00, 0x03 };
	int cnt = ait_cmd(handle, cmd, CMD_FW, sizeof(cmd), DIR_OUT);
	if (cnt < 0) {
		fprintf(stderr, "Error sending FW load command: %s\n", libusb_strerror(cnt));
		goto err;
	}
	/* upload firmware in chunks */
	while (!feof(f)) {
		uint8_t buf[FW_CHUNK_SIZE];
		memset(buf, 0, sizeof(buf));
		cnt = fread(buf, 1, FW_CHUNK_SIZE, f);
		if (cnt < FW_CHUNK_SIZE && ferror(f)) {
			fprintf(stderr, "Error reading firmware file: %s\n", strerror(errno));
			goto err;
		}
		cnt = ait_cmd(handle, buf, CMD_UPLOAD, FW_CHUNK_SIZE, DIR_OUT);
		if (cnt < 0) {
			fprintf(stderr, "Error uploading FW: %s\n", libusb_strerror(cnt));
			goto err;
		}
	}
	/* boot uploaded firmware */
	cmd[1] = 0x01;
	cnt = ait_cmd(handle, cmd, CMD_FW, sizeof(cmd), DIR_OUT);
	if (cnt < 0) {
		fprintf(stderr, "Error sending FW boot command: %s\n", libusb_strerror(cnt));
		goto err;
	}
	ret = true;

err:
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
	fprintf(stderr, " %s -d device_path -f firmware.bin\n", argv[0]);
	fprintf(stderr, "\nLoad firmware to Samsung VG-STC3000, VG-STC4000 or VG-STC5000 TV camera.\n");
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "  -a\tautomatically load firmware to all supported Samsung TV cameras\n");
	fprintf(stderr, "  -d\tUSB device path (/dev/bus/usb/xxx/yyy)\n");
	fprintf(stderr, "  -f\tfirmware file to load (relative to %s if not starting with '/' or '.')\n", FW_DIR);
	return EXIT_FAILURE;
}

int main(int argc, char *argv[]) {
	bool all = false;
	char *device_path = NULL, *firmware_path = NULL;
	int opt;

	while ((opt = getopt(argc, argv, "ad:f:")) != -1) {
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
		default:
			return usage(argv);
		}
	}

	libusb_device_handle *handle;

	if (all) {
		if (device_path || firmware_path) {
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

				if (!ait_upload_fw_uvc(handle, cam->firmware))
					fprintf(stderr, "Firmware upload failed!\n");

				libusb_close(handle);
			}
		}
		if (found < 1)
			fprintf(stderr, "No supported cameras found.\n");
		libusb_free_device_list(list, 1);
	} else {
		if (!device_path || !firmware_path) {
			fprintf(stderr, "Both device and firmware path must be specified!\n");
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

		if (!ait_upload_fw_uvc(handle, firmware_path))
			fprintf(stderr, "Firmware upload failed\n");

		libusb_close(handle);
		close(fd);
	}

	libusb_exit(NULL);
	return 0;
}
