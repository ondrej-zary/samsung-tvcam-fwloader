CFLAGS=-Wall -Wextra --std=c99 -O2

all:	samsung_tvcam_fwload

samsung_tvcam_fwload: samsung_tvcam_fwload.c
	gcc $(CFLAGS) samsung_tvcam_fwload.c `pkg-config --cflags --libs libusb-1.0` -o samsung_tvcam_fwload

clean:
	rm -f samsung_tvcam_fwload

install: samsung_tvcam_fwload
	install -s samsung_tvcam_fwload /lib/udev/
	install -d /lib/firmware/samsung-tvcam
	install raptor_firmware.img FalconFW.bin FalconPlus_FW.bin Heron_Ext_FW.bin /lib/firmware/samsung-tvcam/
	install 40-samsung-tvcam-fwloader.rules /lib/udev/rules.d/
