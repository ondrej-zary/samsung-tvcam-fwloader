// Firmware loader for Samsung TV cameras (Windows version)
// Based on code by Ondrej Zary (2025)
// Adapted for Windows

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <windows.h>
#include "libusb.h"

#define TIMEOUT	1000
#define DIR_IN		0
#define DIR_OUT	1

#define AIT_IDX		0x600
#define AIT_CMD_UPLOAD	0x03
#define AIT_CMD_FW	0x04
#define AIT_FW_CHUNK_SIZE	32

#define FW_DIR	"D:\\firmware\\samsung-tvcam\\"

enum cam_type { CAM_AIT, CAM_MAX };

struct camera {
	char *name;
	uint16_t vendor;
	uint16_t product;
	enum cam_type type;
	char *firmware;
};

static const struct camera cameras[] = {
	{ "VG-STC2000", 0x04e8, 0x2059, CAM_MAX, "raptor_firmware.img" },
	{ "VG-STC3000", 0x04e8, 0x205c, CAM_AIT, "FalconFW.bin" },
	{ "VG-STC4000", 0x04e8, 0x2061, CAM_AIT, "FalconPlus_FW.bin" },
	{ "VG-STC5000", 0x04e8, 0x2065, CAM_AIT, "Heron_Ext_FW.bin" },
	{ NULL }
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
	uint8_t cmd[8] = { 0x01, 0x00, 0x03 };
	int cnt = ait_cmd(handle, cmd, AIT_CMD_FW, sizeof(cmd), DIR_OUT);
	if (cnt < 0) {
		printf("Error sending FW load command: %s\n", libusb_strerror(cnt));
		return false;
	}
	while (!feof(f)) {
		uint8_t buf[AIT_FW_CHUNK_SIZE];
		memset(buf, 0, sizeof(buf));
		cnt = fread(buf, 1, AIT_FW_CHUNK_SIZE, f);
		if (cnt < AIT_FW_CHUNK_SIZE && ferror(f)) {
			printf("Error reading firmware file\n");
			return false;
		}
		cnt = ait_cmd(handle, buf, AIT_CMD_UPLOAD, AIT_FW_CHUNK_SIZE, DIR_OUT);
		if (cnt < 0) {
			printf("Error uploading FW: %s\n", libusb_strerror(cnt));
			return false;
		}
	}
	cmd[1] = 0x01;
	cnt = ait_cmd(handle, cmd, AIT_CMD_FW, sizeof(cmd), DIR_OUT);
	if (cnt < 0) {
		printf("Error sending FW boot command: %s\n", libusb_strerror(cnt));
		return false;
	}
	return true;
}

#define MAX_REQ_FW 0xde
#define MAX_FW_CHUNK_SIZE 4088

static bool max_upload_fw(libusb_device_handle *handle, FILE *f) {
	uint8_t req_type = LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_INTERFACE;
	if (libusb_control_transfer(handle, req_type, MAX_REQ_FW - 1, 0, 0, 0, 0, 0) < 0)
		return false;
	int i = 0;
	while (!feof(f)) {
		uint8_t buf[MAX_FW_CHUNK_SIZE];
		memset(buf, 0, sizeof(buf));
		int cnt = fread(buf, 1, MAX_FW_CHUNK_SIZE, f);
		if (cnt < MAX_FW_CHUNK_SIZE && ferror(f)) {
			printf("Error reading firmware file\n");
			return false;
		}
		if (libusb_control_transfer(handle, req_type, MAX_REQ_FW, i >> 16, i & 0xffff, buf, MAX_FW_CHUNK_SIZE, 0) != MAX_FW_CHUNK_SIZE) {
			printf("Error uploading FW\n");
			return false;
		}
		i++;
	}
	return true;
}

static const struct camera *find_camera(uint16_t vendor, uint16_t product) {
	for (int i = 0; cameras[i].name; i++)
		if (cameras[i].vendor == vendor && cameras[i].product == product)
			return &cameras[i];
	return NULL;
}

static bool upload_fw(libusb_device_handle *handle, const char *filename, enum cam_type type) {
	char path[MAX_PATH];
	if (filename[0] == '/' || filename[0] == '.' || strchr(filename, '\\')) {
		strncpy(path, filename, sizeof(path));
	} else {
		snprintf(path, sizeof(path), "%s%s", FW_DIR, filename);
	}
	FILE *f = fopen(path, "rb");
	if (!f) {
		printf("Error opening firmware file: %s\n", path);
		return false;
	}
	bool ret = (type == CAM_MAX) ? max_upload_fw(handle, f) : ait_upload_fw_uvc(handle, f);
	fclose(f);
	return ret;
}

int main() {
	libusb_context *ctx = NULL;
	libusb_device_handle *handle = NULL;
	libusb_device **list;

	if (libusb_init(&ctx) < 0) {
		printf("Failed to init libusb\n");
		return 1;
	}

	ssize_t cnt = libusb_get_device_list(ctx, &list);
	if (cnt < 0) {
		printf("Could not get device list\n");
		return 1;
	}

	for (ssize_t i = 0; i < cnt; i++) {
		struct libusb_device_descriptor desc;
		if (libusb_get_device_descriptor(list[i], &desc) != 0)
			continue;

		const struct camera *cam = find_camera(desc.idVendor, desc.idProduct);
		if (cam) {
			printf("Found camera %s\n", cam->name);
			if (libusb_open(list[i], &handle) != 0) {
				printf("Failed to open device\n");
				continue;
			}

			if (!upload_fw(handle, cam->firmware, cam->type)) {
				printf("Firmware upload failed for %s\n", cam->name);
			}
			libusb_close(handle);
		}
	}
	libusb_free_device_list(list, 1);
	libusb_exit(ctx);
	return 0;
}
