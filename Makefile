CFLAGS=-Wall -Wextra --std=c99 -O2

all:	ait_fwload

ait_fwload: ait_fwload.c
	gcc $(CFLAGS) ait_fwload.c `pkg-config --cflags --libs libusb-1.0` -o ait_fwload

clean:
	rm -f ait_fwload

install: ait_fwload
	install -s ait_fwload /lib/udev/
	install -d /lib/firmware/samsung-tvcam
	install raptor_firmware.img FalconFW.bin FalconPlus_FW.bin Heron_Ext_FW.bin /lib/firmware/samsung-tvcam/
	install 40-samsung-tvcam-fwloader.rules /lib/udev/rules.d/
