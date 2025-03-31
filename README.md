Firmware loader for Samsung TV cameras
======================================

Samsung used to sell webcams for their smart TVs for the purpose of running
Skype. Skype was discontinued on these TVs in 2016 and these cameras became
useless.

When connected to a PC, the camera identifies as unknown device or as an UVC
webcam but does not work. That's because it's missing firmware. Camera
contains only a bootloader and TV uploads firmware to camera's RAM each time
it's plugged in.

This project is a firmware loader that allows Samsung TV cameras to be used
in Linux (and possibly other OSs).

Camera type | USB ID    | Firmware            | Chip type  | Status
------------|-----------|---------------------|------------|-------------------------------------------
CY-STC1100  | 04e8:2058 | mgfw_2058.bin       |            | not supported, probably not UVC compatible
VG-STC2000  | 04e8:2059 | raptor_firmware.img | MAX	   | works
VG-STC3000  | 04e8:205c | FalconFW.bin        | AIT	   | works
VG-STC4000  | 04e8:2061 | FalconPlus_FW.bin   | AIT	   | works
VG-STC5000  | 04e8:2065 | Heron_Ext_FW.bin    | AIT	   | works

Compiling from source
---------------------
Requirements: make, gcc, libusb-1.0-0-dev

To compile, simply run "make":

    $ make

To install compiled loaders and udev rules, run "make install" as root:

    # make install

or

    $ sudo make install

You can then (re)plug the camera and it should work like any other UVC camera.

Samsung implementation
----------------------
On Samsung TVs, camera support is provided by EMP (External Module Process)
called empCamera. The EMPs are specific to TV model/firmware and can updated
automatically from web by the TV. It fetches XML list of EMPs, e.g.:
 * http://oempprd.samsungcloudsolution.com/emp/empinfo_X10P_0.970.xml (ES series TVs)
 * http://oempprd.samsungcloudsolution.com/emp/empinfo_X14_4.400.xml (H series TVs)

The interesting files are:
 * http://oempprd.samsungcloudsolution.net/emp/emp/X10P_0.970_empCamera_VER_1.140419.zip
 * http://oempprd.samsungcloudsolution.com/emp/emp/X14_4.400_empCamera_VER_4.150303.img

These EMPs contain libraries to handle 5 camera types (the number at the end
matches camera's USB PID):
 * libMoIPCamera_2058.so
 * libMoIPCamera_2059.so
 * libMoIPCamera_205c.so
 * libMoIPCamera_2061.so
 * libMoIPCamera_2065.so

These libraries initialize and also control the camera. We only need the
initialization part - once the camera is working as an UVC camera, the
kernel driver and userspace apps take care of the control.

Camera HW details
-----------------
All cameras provide hardware H.264 encoding.

### CY-STC1100
 * 1280x720/30fps
 * Maxim MG3500 HD H.264 Codec SoC
 * OmniVision OV2640 sensor (1600x1200)
 * Wolfson WM8750L audio codec
 * probably not UVC compatible

lsusb (no firmware loaded):

    Bus 002 Device 008: ID 04e8:2058 Samsung Electronics Co., Ltd
    Device Descriptor:
      bLength                18
      bDescriptorType         1
      bcdUSB               2.00
      bDeviceClass          255 Vendor Specific Class
      bDeviceSubClass       255 Vendor Specific Subclass
      bDeviceProtocol       255 Vendor Specific Protocol
      bMaxPacketSize0        64
      idVendor           0x04e8 Samsung Electronics Co., Ltd
      idProduct          0x2058
      bcdDevice            0.00
      iManufacturer           1
      iProduct                2
      iSerial                 3
      bNumConfigurations      1
      Configuration Descriptor:
        bLength                 9
        bDescriptorType         2
        wTotalLength           53
        bNumInterfaces          1
        bConfigurationValue     1
        iConfiguration          0
        bmAttributes         0x80
          (Bus Powered)
        MaxPower              400mA
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        0
          bAlternateSetting       0
          bNumEndpoints           5
          bInterfaceClass       255 Vendor Specific Class
          bInterfaceSubClass    255 Vendor Specific Subclass
          bInterfaceProtocol    255 Vendor Specific Protocol
          iInterface              0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x01  EP 1 OUT
            bmAttributes            2
              Transfer Type            Bulk
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0040  1x 64 bytes
            bInterval               0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x81  EP 1 IN
            bmAttributes            2
              Transfer Type            Bulk
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0040  1x 64 bytes
            bInterval               0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x02  EP 2 OUT
            bmAttributes            2
              Transfer Type            Bulk
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0200  1x 512 bytes
            bInterval               0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x86  EP 6 IN
            bmAttributes            2
              Transfer Type            Bulk
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0200  1x 512 bytes
            bInterval               0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x88  EP 8 IN
            bmAttributes            2
              Transfer Type            Bulk
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0040  1x 64 bytes
            bInterval               0

### VG-STC2000
 * 1280x720/30fps
 * Maxim MAX64380
 * 128MB RAM
 * BT656 sensor

lsusb (no firmware loaded):

    Bus 001 Device 010: ID 04e8:2059 Samsung Electronics Co., Ltd
    Device Descriptor:
      bLength                18
      bDescriptorType         1
      bcdUSB               2.00
      bDeviceClass          255 Vendor Specific Class
      bDeviceSubClass         0
      bDeviceProtocol         0
      bMaxPacketSize0        64
      idVendor           0x04e8 Samsung Electronics Co., Ltd
      idProduct          0x2059
      bcdDevice            0.00
      iManufacturer           0
      iProduct                0
      iSerial                 0
      bNumConfigurations      1
      Configuration Descriptor:
        bLength                 9
        bDescriptorType         2
        wTotalLength       0x0012
        bNumInterfaces          1
        bConfigurationValue     1
        iConfiguration          0
        bmAttributes         0xc0
          Self Powered
        MaxPower                2mA
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        0
          bAlternateSetting       0
          bNumEndpoints           0
          bInterfaceClass       255 Vendor Specific Class
          bInterfaceSubClass      1
          bInterfaceProtocol      1
          iInterface              9 M
    Device Qualifier (for other device speed):
      bLength                10
      bDescriptorType         6
      bcdUSB               2.00
      bDeviceClass          255 Vendor Specific Class
      bDeviceSubClass       255 Vendor Specific Subclass
      bDeviceProtocol       255 Vendor Specific Protocol
      bMaxPacketSize0        64
      bNumConfigurations      1

lsusb (firmware loaded):

    Bus 001 Device 011: ID 04e8:205a Samsung Electronics Co., Ltd MAX64380
    Device Descriptor:
      bLength                18
      bDescriptorType         1
      bcdUSB               2.00
      bDeviceClass          239 Miscellaneous Device
      bDeviceSubClass         2
      bDeviceProtocol         1 Interface Association
      bMaxPacketSize0        64
      idVendor           0x04e8 Samsung Electronics Co., Ltd
      idProduct          0x205a
      bcdDevice            0.00
      iManufacturer           1 Maxim
      iProduct                2 MAX64380
      iSerial                 3 12345
      bNumConfigurations      2
      Configuration Descriptor:
        bLength                 9
        bDescriptorType         2
        wTotalLength       0x053d
        bNumInterfaces          6
        bConfigurationValue     1
        iConfiguration          0
        bmAttributes         0x80
          (Bus Powered)
        MaxPower              400mA
        Interface Association:
          bLength                 8
          bDescriptorType        11
          bFirstInterface         0
          bInterfaceCount         2
          bFunctionClass         14 Video
          bFunctionSubClass       3 Video Interface Collection
          bFunctionProtocol       0
          iFunction               2 MAX64380
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        0
          bAlternateSetting       0
          bNumEndpoints           1
          bInterfaceClass        14 Video
          bInterfaceSubClass      1 Video Control
          bInterfaceProtocol      0
          iInterface              2 MAX64380
          VideoControl Interface Descriptor:
            bLength                13
            bDescriptorType        36
            bDescriptorSubtype      1 (HEADER)
            bcdUVC               1.10
            wTotalLength       0x006f
            dwClockFrequency        6.000000MHz
            bInCollection           1
            baInterfaceNr( 0)       1
          VideoControl Interface Descriptor:
            bLength                18
            bDescriptorType        36
            bDescriptorSubtype      2 (INPUT_TERMINAL)
            bTerminalID             1
            wTerminalType      0x0201 Camera Sensor
            bAssocTerminal          0
            iTerminal               0
            wObjectiveFocalLengthMin      0
            wObjectiveFocalLengthMax      0
            wOcularFocalLength            0
            bControlSize                  3
            bmControls           0x00000800
              PanTilt (Absolute)
          VideoControl Interface Descriptor:
            bLength                13
            bDescriptorType        36
            bDescriptorSubtype      5 (PROCESSING_UNIT)
            bUnitID                 4
            bSourceID               1
            wMaxMultiplier        100
            bControlSize            3
            bmControls     0x0000463f
              Brightness
              Contrast
              Hue
              Saturation
              Sharpness
              Gamma
              Gain
              Power Line Frequency
              Digital Multiplier
            iProcessing             0
            bmVideoStandards     0x02
              NTSC - 525/60
          VideoControl Interface Descriptor:
            bLength                29
            bDescriptorType        36
            bDescriptorSubtype      6 (EXTENSION_UNIT)
            bUnitID                 5
            guidExtensionCode         {ba2b92d9-26f2-4294-42ae-e4eb4d68dd06}
            bNumControls           14
            bNrInPins               1
            baSourceID( 0)          4
            bControlSize            4
            bmControls( 0)       0xff
            bmControls( 1)       0x3f
            bmControls( 2)       0x00
            bmControls( 3)       0x00
            iExtension              0
          VideoControl Interface Descriptor:
            bLength                29
            bDescriptorType        36
            bDescriptorSubtype      6 (EXTENSION_UNIT)
            bUnitID                 6
            guidExtensionCode         {df5dcd12-7d5f-4bba-bb6d-4b625add5272}
            bNumControls           19
            bNrInPins               1
            baSourceID( 0)          4
            bControlSize            4
            bmControls( 0)       0xff
            bmControls( 1)       0x87
            bmControls( 2)       0x07
            bmControls( 3)       0x00
            iExtension              0
          VideoControl Interface Descriptor:
            bLength                 9
            bDescriptorType        36
            bDescriptorSubtype      3 (OUTPUT_TERMINAL)
            bTerminalID             2
            wTerminalType      0x0101 USB Streaming
            bAssocTerminal          0
            bSourceID               5
            iTerminal               0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x84  EP 4 IN
            bmAttributes            3
              Transfer Type            Interrupt
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0010  1x 16 bytes
            bInterval               9
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        1
          bAlternateSetting       0
          bNumEndpoints           0
          bInterfaceClass        14 Video
          bInterfaceSubClass      2 Video Streaming
          bInterfaceProtocol      0
          iInterface              0
          VideoStreaming Interface Descriptor:
            bLength                            15
            bDescriptorType                    36
            bDescriptorSubtype                  1 (INPUT_HEADER)
            bNumFormats                         2
            wTotalLength                   0x006e
            bEndpointAddress                 0x82  EP 2 IN
            bmInfo                              0
            bTerminalLink                       2
            bStillCaptureMethod                 0
            bTriggerSupport                     0
            bTriggerUsage                       0
            bControlSize                        1
            bmaControls( 0)                     0
            bmaControls( 1)                     0
          VideoStreaming Interface Descriptor:
            bLength                            23
            bDescriptorType                    36
            bDescriptorSubtype                 10 (FORMAT_MPEG2TS)
            bFormatIndex                        1
            bDataOffset                         0
            bPacketLength                     188
            bStrideLength                     188
            guidStrideFormat                      {00000000-0000-0000-0000-000000000000}
          VideoStreaming Interface Descriptor:
            bLength                            28
            bDescriptorType                    36
            bDescriptorSubtype                 16 (FORMAT_FRAME_BASED)
            bFormatIndex                        2
            bNumFrameDescriptors                1
            guidFormat                            {34363248-0000-0010-8000-00aa00389b71}
            bBitsPerPixel                      12
            bDefaultFrameIndex                  1
            bAspectRatioX                      16
            bAspectRatioY                       9
            bmInterlaceFlags                 0x00
              Interlaced stream or variable: No
              Fields per frame: 2 fields
              Field 1 first: No
              Field pattern: Field 1 only
            bCopyProtect                        0
            bVariableSize                     1
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                 17 (FRAME_FRAME_BASED)
            bFrameIndex                         1
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           1280
            wHeight                           720
            dwMinBitRate                     1000
            dwMaxBitRate                 10000000
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  0
            dwBytesPerLine                      0
            dwMinFrameInterval             333333
            dwMaxFrameInterval           10000000
            dwFrameIntervalStep                 1
          VideoStreaming Interface Descriptor:
            bLength                             6
            bDescriptorType                    36
            bDescriptorSubtype                 13 (COLORFORMAT)
            bColorPrimaries                     1 (BT.709,sRGB)
            bTransferCharacteristics            1 (BT.709)
            bMatrixCoefficients                 4 (SMPTE 170M (BT.601))
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        1
          bAlternateSetting       1
          bNumEndpoints           1
          bInterfaceClass        14 Video
          bInterfaceSubClass      2 Video Streaming
          bInterfaceProtocol      0
          iInterface              0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x82  EP 2 IN
            bmAttributes            5
              Transfer Type            Isochronous
              Synch Type               Asynchronous
              Usage Type               Data
            wMaxPacketSize     0x017a  1x 378 bytes
            bInterval               1
        Interface Association:
          bLength                 8
          bDescriptorType        11
          bFirstInterface         2
          bInterfaceCount         2
          bFunctionClass         14 Video
          bFunctionSubClass       3 Video Interface Collection
          bFunctionProtocol       0
          iFunction               2 MAX64380
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        2
          bAlternateSetting       0
          bNumEndpoints           1
          bInterfaceClass        14 Video
          bInterfaceSubClass      1 Video Control
          bInterfaceProtocol      0
          iInterface              2 MAX64380
          VideoControl Interface Descriptor:
            bLength                13
            bDescriptorType        36
            bDescriptorSubtype      1 (HEADER)
            bcdUVC               1.10
            wTotalLength       0x006f
            dwClockFrequency        6.000000MHz
            bInCollection           1
            baInterfaceNr( 0)       3
          VideoControl Interface Descriptor:
            bLength                18
            bDescriptorType        36
            bDescriptorSubtype      2 (INPUT_TERMINAL)
            bTerminalID             1
            wTerminalType      0x0201 Camera Sensor
            bAssocTerminal          0
            iTerminal               0
            wObjectiveFocalLengthMin      0
            wObjectiveFocalLengthMax      0
            wOcularFocalLength            0
            bControlSize                  3
            bmControls           0x00000800
              PanTilt (Absolute)
          VideoControl Interface Descriptor:
            bLength                13
            bDescriptorType        36
            bDescriptorSubtype      5 (PROCESSING_UNIT)
            bUnitID                 4
            bSourceID               1
            wMaxMultiplier        100
            bControlSize            3
            bmControls     0x0000463f
              Brightness
              Contrast
              Hue
              Saturation
              Sharpness
              Gamma
              Gain
              Power Line Frequency
              Digital Multiplier
            iProcessing             0
            bmVideoStandards     0x02
              NTSC - 525/60
          VideoControl Interface Descriptor:
            bLength                29
            bDescriptorType        36
            bDescriptorSubtype      6 (EXTENSION_UNIT)
            bUnitID                 5
            guidExtensionCode         {ba2b92d9-26f2-4294-42ae-e4eb4d68dd06}
            bNumControls           14
            bNrInPins               1
            baSourceID( 0)          4
            bControlSize            4
            bmControls( 0)       0xff
            bmControls( 1)       0x2f
            bmControls( 2)       0x00
            bmControls( 3)       0x00
            iExtension              0
          VideoControl Interface Descriptor:
            bLength                29
            bDescriptorType        36
            bDescriptorSubtype      6 (EXTENSION_UNIT)
            bUnitID                 6
            guidExtensionCode         {df5dcd12-7d5f-4bba-bb6d-4b625add5272}
            bNumControls           19
            bNrInPins               1
            baSourceID( 0)          4
            bControlSize            4
            bmControls( 0)       0xff
            bmControls( 1)       0x87
            bmControls( 2)       0x07
            bmControls( 3)       0x00
            iExtension              0
          VideoControl Interface Descriptor:
            bLength                 9
            bDescriptorType        36
            bDescriptorSubtype      3 (OUTPUT_TERMINAL)
            bTerminalID             2
            wTerminalType      0x0101 USB Streaming
            bAssocTerminal          0
            bSourceID               5
            iTerminal               0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x85  EP 5 IN
            bmAttributes            3
              Transfer Type            Interrupt
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0008  1x 8 bytes
            bInterval               9
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        3
          bAlternateSetting       0
          bNumEndpoints           0
          bInterfaceClass        14 Video
          bInterfaceSubClass      2 Video Streaming
          bInterfaceProtocol      0
          iInterface              0
          VideoStreaming Interface Descriptor:
            bLength                            17
            bDescriptorType                    36
            bDescriptorSubtype                  1 (INPUT_HEADER)
            bNumFormats                         4
            wTotalLength                   0x028a
            bEndpointAddress                 0x83  EP 3 IN
            bmInfo                              0
            bTerminalLink                       2
            bStillCaptureMethod                 0
            bTriggerSupport                     0
            bTriggerUsage                       0
            bControlSize                        1
            bmaControls( 0)                     0
            bmaControls( 1)                     4
            bmaControls( 2)                     0
            bmaControls( 3)                     0
          VideoStreaming Interface Descriptor:
            bLength                            27
            bDescriptorType                    36
            bDescriptorSubtype                  4 (FORMAT_UNCOMPRESSED)
            bFormatIndex                        1
            bNumFrameDescriptors                6
            guidFormat                            {32595559-0000-0010-8000-00aa00389b71}
            bBitsPerPixel                      16
            bDefaultFrameIndex                  1
            bAspectRatioX                       0
            bAspectRatioY                       0
            bmInterlaceFlags                 0x00
              Interlaced stream or variable: No
              Fields per frame: 2 fields
              Field 1 first: No
              Field pattern: Field 1 only
            bCopyProtect                        0
          VideoStreaming Interface Descriptor:
            bLength                            46
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         1
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            160
            wHeight                           120
            dwMinBitRate                  3072000
            dwMaxBitRate                  9216000
            dwMaxVideoFrameBufferSize       38400
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  5
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            400000
            dwFrameInterval( 2)            500000
            dwFrameInterval( 3)            666666
            dwFrameInterval( 4)           1000000
          VideoStreaming Interface Descriptor:
            bLength                            46
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         2
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            320
            wHeight                           240
            dwMinBitRate                 12288000
            dwMaxBitRate                 36864000
            dwMaxVideoFrameBufferSize      153600
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  5
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            400000
            dwFrameInterval( 2)            500000
            dwFrameInterval( 3)            666666
            dwFrameInterval( 4)           1000000
          VideoStreaming Interface Descriptor:
            bLength                            46
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         3
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            432
            wHeight                           240
            dwMinBitRate                 16588800
            dwMaxBitRate                 49766400
            dwMaxVideoFrameBufferSize      207360
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  5
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            400000
            dwFrameInterval( 2)            500000
            dwFrameInterval( 3)            666666
            dwFrameInterval( 4)           1000000
          VideoStreaming Interface Descriptor:
            bLength                            46
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         4
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            624
            wHeight                           352
            dwMinBitRate                 35143680
            dwMaxBitRate                105431040
            dwMaxVideoFrameBufferSize      439296
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  5
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            400000
            dwFrameInterval( 2)            500000
            dwFrameInterval( 3)            666666
            dwFrameInterval( 4)           1000000
          VideoStreaming Interface Descriptor:
            bLength                            46
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         5
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            640
            wHeight                           480
            dwMinBitRate                 49152000
            dwMaxBitRate                147456000
            dwMaxVideoFrameBufferSize      614400
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  5
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            400000
            dwFrameInterval( 2)            500000
            dwFrameInterval( 3)            666666
            dwFrameInterval( 4)           1000000
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         6
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           1280
            wHeight                           720
            dwMinBitRate                 58982400
            dwMaxBitRate                 58982400
            dwMaxVideoFrameBufferSize     1843200
            dwDefaultFrameInterval        2500000
            bFrameIntervalType                  1
            dwFrameInterval( 0)           2500000
          VideoStreaming Interface Descriptor:
            bLength                             6
            bDescriptorType                    36
            bDescriptorSubtype                 13 (COLORFORMAT)
            bColorPrimaries                     1 (BT.709,sRGB)
            bTransferCharacteristics            1 (BT.709)
            bMatrixCoefficients                 4 (SMPTE 170M (BT.601))
          VideoStreaming Interface Descriptor:
            bLength                            11
            bDescriptorType                    36
            bDescriptorSubtype                  6 (FORMAT_MJPEG)
            bFormatIndex                        2
            bNumFrameDescriptors                6
            bFlags                              0
              Fixed-size samples: No
            bDefaultFrameIndex                  1
            bAspectRatioX                       0
            bAspectRatioY                       0
            bmInterlaceFlags                 0x00
              Interlaced stream or variable: No
              Fields per frame: 1 fields
              Field 1 first: No
              Field pattern: Field 1 only
            bCopyProtect                        0
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         1
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            640
            wHeight                           480
            dwMinBitRate                  4194304
            dwMaxBitRate                  5767168
            dwMaxVideoFrameBufferSize       65536
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  0
            dwMinFrameInterval             333333
            dwMaxFrameInterval           10000000
            dwFrameIntervalStep                 1
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         2
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            624
            wHeight                           352
            dwMinBitRate                  4194304
            dwMaxBitRate                  5767168
            dwMaxVideoFrameBufferSize       65536
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  0
            dwMinFrameInterval             333333
            dwMaxFrameInterval           10000000
            dwFrameIntervalStep                 1
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         3
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            320
            wHeight                           240
            dwMinBitRate                  1048576
            dwMaxBitRate                  1441792
            dwMaxVideoFrameBufferSize       65536
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  0
            dwMinFrameInterval             333333
            dwMaxFrameInterval           10000000
            dwFrameIntervalStep                 1
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         4
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            432
            wHeight                           240
            dwMinBitRate                  1048576
            dwMaxBitRate                  1441792
            dwMaxVideoFrameBufferSize       65536
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  0
            dwMinFrameInterval             333333
            dwMaxFrameInterval           10000000
            dwFrameIntervalStep                 1
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         5
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           1280
            wHeight                           720
            dwMinBitRate                  6291456
            dwMaxBitRate                  8388608
            dwMaxVideoFrameBufferSize       98304
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  0
            dwMinFrameInterval             333333
            dwMaxFrameInterval           10000000
            dwFrameIntervalStep                 1
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         6
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            960
            wHeight                           720
            dwMinBitRate                  6291456
            dwMaxBitRate                  8388608
            dwMaxVideoFrameBufferSize       98304
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  0
            dwMinFrameInterval             333333
            dwMaxFrameInterval           10000000
            dwFrameIntervalStep                 1
          VideoStreaming Interface Descriptor:
            bLength                             6
            bDescriptorType                    36
            bDescriptorSubtype                 13 (COLORFORMAT)
            bColorPrimaries                     1 (BT.709,sRGB)
            bTransferCharacteristics            1 (BT.709)
            bMatrixCoefficients                 4 (SMPTE 170M (BT.601))
          VideoStreaming Interface Descriptor:
            bLength                            23
            bDescriptorType                    36
            bDescriptorSubtype                 10 (FORMAT_MPEG2TS)
            bFormatIndex                        3
            bDataOffset                         0
            bPacketLength                     188
            bStrideLength                     188
            guidStrideFormat                      {00000000-0000-0000-0000-000000000000}
          VideoStreaming Interface Descriptor:
            bLength                            28
            bDescriptorType                    36
            bDescriptorSubtype                 16 (FORMAT_FRAME_BASED)
            bFormatIndex                        4
            bNumFrameDescriptors                1
            guidFormat                            {34363248-0000-0010-8000-00aa00389b71}
            bBitsPerPixel                      12
            bDefaultFrameIndex                  1
            bAspectRatioX                      16
            bAspectRatioY                       9
            bmInterlaceFlags                 0x00
              Interlaced stream or variable: No
              Fields per frame: 2 fields
              Field 1 first: No
              Field pattern: Field 1 only
            bCopyProtect                        0
            bVariableSize                     1
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                 17 (FRAME_FRAME_BASED)
            bFrameIndex                         1
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           1280
            wHeight                           720
            dwMinBitRate                     1000
            dwMaxBitRate                 10000000
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  0
            dwBytesPerLine                      0
            dwMinFrameInterval             333333
            dwMaxFrameInterval           10000000
            dwFrameIntervalStep                 1
          VideoStreaming Interface Descriptor:
            bLength                             6
            bDescriptorType                    36
            bDescriptorSubtype                 13 (COLORFORMAT)
            bColorPrimaries                     1 (BT.709,sRGB)
            bTransferCharacteristics            1 (BT.709)
            bMatrixCoefficients                 4 (SMPTE 170M (BT.601))
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        3
          bAlternateSetting       1
          bNumEndpoints           1
          bInterfaceClass        14 Video
          bInterfaceSubClass      2 Video Streaming
          bInterfaceProtocol      0
          iInterface              0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x83  EP 3 IN
            bmAttributes            5
              Transfer Type            Isochronous
              Synch Type               Asynchronous
              Usage Type               Data
            wMaxPacketSize     0x1400  3x 1024 bytes
            bInterval               1
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        3
          bAlternateSetting       2
          bNumEndpoints           1
          bInterfaceClass        14 Video
          bInterfaceSubClass      2 Video Streaming
          bInterfaceProtocol      0
          iInterface              0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x83  EP 3 IN
            bmAttributes            5
              Transfer Type            Isochronous
              Synch Type               Asynchronous
              Usage Type               Data
            wMaxPacketSize     0x0400  1x 1024 bytes
            bInterval               1
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        3
          bAlternateSetting       3
          bNumEndpoints           1
          bInterfaceClass        14 Video
          bInterfaceSubClass      2 Video Streaming
          bInterfaceProtocol      0
          iInterface              0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x83  EP 3 IN
            bmAttributes            5
              Transfer Type            Isochronous
              Synch Type               Asynchronous
              Usage Type               Data
            wMaxPacketSize     0x0200  1x 512 bytes
            bInterval               1
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        3
          bAlternateSetting       4
          bNumEndpoints           1
          bInterfaceClass        14 Video
          bInterfaceSubClass      2 Video Streaming
          bInterfaceProtocol      0
          iInterface              0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x83  EP 3 IN
            bmAttributes            5
              Transfer Type            Isochronous
              Synch Type               Asynchronous
              Usage Type               Data
            wMaxPacketSize     0x017a  1x 378 bytes
            bInterval               1
        Interface Association:
          bLength                 8
          bDescriptorType        11
          bFirstInterface         4
          bInterfaceCount         2
          bFunctionClass          1 Audio
          bFunctionSubClass       2 Streaming
          bFunctionProtocol       0
          iFunction               2 MAX64380
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        4
          bAlternateSetting       0
          bNumEndpoints           0
          bInterfaceClass         1 Audio
          bInterfaceSubClass      1 Control Device
          bInterfaceProtocol      0
          iInterface              0
          AudioControl Interface Descriptor:
            bLength                 9
            bDescriptorType        36
            bDescriptorSubtype      1 (HEADER)
            bcdADC               1.00
            wTotalLength       0x0027
            bInCollection           1
            baInterfaceNr(0)        5
          AudioControl Interface Descriptor:
            bLength                12
            bDescriptorType        36
            bDescriptorSubtype      2 (INPUT_TERMINAL)
            bTerminalID             1
            wTerminalType      0x0201 Microphone
            bAssocTerminal          0
            bNrChannels             2
            wChannelConfig     0x0003
              Left Front (L)
              Right Front (R)
            iChannelNames           0
            iTerminal               0
          AudioControl Interface Descriptor:
            bLength                 9
            bDescriptorType        36
            bDescriptorSubtype      6 (FEATURE_UNIT)
            bUnitID                 2
            bSourceID               1
            bControlSize            1
            bmaControls(0)       0x01
              Mute Control
            bmaControls(1)       0x02
              Volume Control
            iFeature                0
          AudioControl Interface Descriptor:
            bLength                 9
            bDescriptorType        36
            bDescriptorSubtype      3 (OUTPUT_TERMINAL)
            bTerminalID             3
            wTerminalType      0x0101 USB Streaming
            bAssocTerminal          0
            bSourceID               2
            iTerminal               0
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        5
          bAlternateSetting       0
          bNumEndpoints           0
          bInterfaceClass         1 Audio
          bInterfaceSubClass      2 Streaming
          bInterfaceProtocol      0
          iInterface              0
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        5
          bAlternateSetting       1
          bNumEndpoints           1
          bInterfaceClass         1 Audio
          bInterfaceSubClass      2 Streaming
          bInterfaceProtocol      0
          iInterface              0
          AudioStreaming Interface Descriptor:
            bLength                 7
            bDescriptorType        36
            bDescriptorSubtype      1 (AS_GENERAL)
            bTerminalLink           3
            bDelay                  1 frames
            wFormatTag         0x0001 PCM
          AudioStreaming Interface Descriptor:
            bLength                11
            bDescriptorType        36
            bDescriptorSubtype      2 (FORMAT_TYPE)
            bFormatType             1 (FORMAT_TYPE_I)
            bNrChannels             2
            bSubframeSize           2
            bBitResolution         16
            bSamFreqType            1 Discrete
            tSamFreq[ 0]         8000
          Endpoint Descriptor:
            bLength                 9
            bDescriptorType         5
            bEndpointAddress     0x81  EP 1 IN
            bmAttributes            5
              Transfer Type            Isochronous
              Synch Type               Asynchronous
              Usage Type               Data
            wMaxPacketSize     0x00c0  1x 192 bytes
            bInterval               4
            bRefresh                0
            bSynchAddress           0
            AudioStreaming Endpoint Descriptor:
              bLength                 7
              bDescriptorType        37
              bDescriptorSubtype      1 (EP_GENERAL)
              bmAttributes         0x00
              bLockDelayUnits         0 Undefined
              wLockDelay         0x0000
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        5
          bAlternateSetting       2
          bNumEndpoints           1
          bInterfaceClass         1 Audio
          bInterfaceSubClass      2 Streaming
          bInterfaceProtocol      0
          iInterface              0
          AudioStreaming Interface Descriptor:
            bLength                 7
            bDescriptorType        36
            bDescriptorSubtype      1 (AS_GENERAL)
            bTerminalLink           3
            bDelay                  1 frames
            wFormatTag         0x0001 PCM
          AudioStreaming Interface Descriptor:
            bLength                11
            bDescriptorType        36
            bDescriptorSubtype      2 (FORMAT_TYPE)
            bFormatType             1 (FORMAT_TYPE_I)
            bNrChannels             2
            bSubframeSize           2
            bBitResolution         16
            bSamFreqType            1 Discrete
            tSamFreq[ 0]        16000
          Endpoint Descriptor:
            bLength                 9
            bDescriptorType         5
            bEndpointAddress     0x81  EP 1 IN
            bmAttributes            5
              Transfer Type            Isochronous
              Synch Type               Asynchronous
              Usage Type               Data
            wMaxPacketSize     0x00c0  1x 192 bytes
            bInterval               4
            bRefresh                0
            bSynchAddress           0
            AudioStreaming Endpoint Descriptor:
              bLength                 7
              bDescriptorType        37
              bDescriptorSubtype      1 (EP_GENERAL)
              bmAttributes         0x00
              bLockDelayUnits         0 Undefined
              wLockDelay         0x0000
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        5
          bAlternateSetting       3
          bNumEndpoints           1
          bInterfaceClass         1 Audio
          bInterfaceSubClass      2 Streaming
          bInterfaceProtocol      0
          iInterface              0
          AudioStreaming Interface Descriptor:
            bLength                 7
            bDescriptorType        36
            bDescriptorSubtype      1 (AS_GENERAL)
            bTerminalLink           3
            bDelay                  1 frames
            wFormatTag         0x0001 PCM
          AudioStreaming Interface Descriptor:
            bLength                11
            bDescriptorType        36
            bDescriptorSubtype      2 (FORMAT_TYPE)
            bFormatType             1 (FORMAT_TYPE_I)
            bNrChannels             2
            bSubframeSize           2
            bBitResolution         16
            bSamFreqType            1 Discrete
            tSamFreq[ 0]        24000
          Endpoint Descriptor:
            bLength                 9
            bDescriptorType         5
            bEndpointAddress     0x81  EP 1 IN
            bmAttributes            5
              Transfer Type            Isochronous
              Synch Type               Asynchronous
              Usage Type               Data
            wMaxPacketSize     0x00c0  1x 192 bytes
            bInterval               4
            bRefresh                0
            bSynchAddress           0
            AudioStreaming Endpoint Descriptor:
              bLength                 7
              bDescriptorType        37
              bDescriptorSubtype      1 (EP_GENERAL)
              bmAttributes         0x00
              bLockDelayUnits         0 Undefined
              wLockDelay         0x0000
      Configuration Descriptor:
        bLength                 9
        bDescriptorType         2
        wTotalLength       0x04fb
        bNumInterfaces          6
        bConfigurationValue     2
        iConfiguration          0
        bmAttributes         0x80
          (Bus Powered)
        MaxPower              400mA
        Interface Association:
          bLength                 8
          bDescriptorType        11
          bFirstInterface         0
          bInterfaceCount         2
          bFunctionClass         14 Video
          bFunctionSubClass       3 Video Interface Collection
          bFunctionProtocol       0
          iFunction               2 MAX64380
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        0
          bAlternateSetting       0
          bNumEndpoints           1
          bInterfaceClass        14 Video
          bInterfaceSubClass      1 Video Control
          bInterfaceProtocol      0
          iInterface              2 MAX64380
          VideoControl Interface Descriptor:
            bLength                13
            bDescriptorType        36
            bDescriptorSubtype      1 (HEADER)
            bcdUVC               1.10
            wTotalLength       0x006f
            dwClockFrequency        6.000000MHz
            bInCollection           1
            baInterfaceNr( 0)       1
          VideoControl Interface Descriptor:
            bLength                18
            bDescriptorType        36
            bDescriptorSubtype      2 (INPUT_TERMINAL)
            bTerminalID             1
            wTerminalType      0x0201 Camera Sensor
            bAssocTerminal          0
            iTerminal               0
            wObjectiveFocalLengthMin      0
            wObjectiveFocalLengthMax      0
            wOcularFocalLength            0
            bControlSize                  3
            bmControls           0x00000800
              PanTilt (Absolute)
          VideoControl Interface Descriptor:
            bLength                13
            bDescriptorType        36
            bDescriptorSubtype      5 (PROCESSING_UNIT)
            bUnitID                 4
            bSourceID               1
            wMaxMultiplier        100
            bControlSize            3
            bmControls     0x0000463f
              Brightness
              Contrast
              Hue
              Saturation
              Sharpness
              Gamma
              Gain
              Power Line Frequency
              Digital Multiplier
            iProcessing             0
            bmVideoStandards     0x02
              NTSC - 525/60
          VideoControl Interface Descriptor:
            bLength                29
            bDescriptorType        36
            bDescriptorSubtype      6 (EXTENSION_UNIT)
            bUnitID                 5
            guidExtensionCode         {ba2b92d9-26f2-4294-42ae-e4eb4d68dd06}
            bNumControls           14
            bNrInPins               1
            baSourceID( 0)          4
            bControlSize            4
            bmControls( 0)       0xff
            bmControls( 1)       0x3f
            bmControls( 2)       0x00
            bmControls( 3)       0x00
            iExtension              0
          VideoControl Interface Descriptor:
            bLength                29
            bDescriptorType        36
            bDescriptorSubtype      6 (EXTENSION_UNIT)
            bUnitID                 6
            guidExtensionCode         {df5dcd12-7d5f-4bba-bb6d-4b625add5272}
            bNumControls           19
            bNrInPins               1
            baSourceID( 0)          4
            bControlSize            4
            bmControls( 0)       0xff
            bmControls( 1)       0x87
            bmControls( 2)       0x07
            bmControls( 3)       0x00
            iExtension              0
          VideoControl Interface Descriptor:
            bLength                 9
            bDescriptorType        36
            bDescriptorSubtype      3 (OUTPUT_TERMINAL)
            bTerminalID             2
            wTerminalType      0x0101 USB Streaming
            bAssocTerminal          0
            bSourceID               5
            iTerminal               0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x84  EP 4 IN
            bmAttributes            3
              Transfer Type            Interrupt
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0010  1x 16 bytes
            bInterval               9
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        1
          bAlternateSetting       0
          bNumEndpoints           1
          bInterfaceClass        14 Video
          bInterfaceSubClass      2 Video Streaming
          bInterfaceProtocol      0
          iInterface              0
          VideoStreaming Interface Descriptor:
            bLength                            15
            bDescriptorType                    36
            bDescriptorSubtype                  1 (INPUT_HEADER)
            bNumFormats                         2
            wTotalLength                   0x006e
            bEndpointAddress                 0x82  EP 2 IN
            bmInfo                              0
            bTerminalLink                       2
            bStillCaptureMethod                 0
            bTriggerSupport                     0
            bTriggerUsage                       0
            bControlSize                        1
            bmaControls( 0)                     0
            bmaControls( 1)                     0
          VideoStreaming Interface Descriptor:
            bLength                            23
            bDescriptorType                    36
            bDescriptorSubtype                 10 (FORMAT_MPEG2TS)
            bFormatIndex                        1
            bDataOffset                         0
            bPacketLength                     188
            bStrideLength                     188
            guidStrideFormat                      {00000000-0000-0000-0000-000000000000}
          VideoStreaming Interface Descriptor:
            bLength                            28
            bDescriptorType                    36
            bDescriptorSubtype                 16 (FORMAT_FRAME_BASED)
            bFormatIndex                        2
            bNumFrameDescriptors                1
            guidFormat                            {34363248-0000-0010-8000-00aa00389b71}
            bBitsPerPixel                      12
            bDefaultFrameIndex                  1
            bAspectRatioX                      16
            bAspectRatioY                       9
            bmInterlaceFlags                 0x00
              Interlaced stream or variable: No
              Fields per frame: 2 fields
              Field 1 first: No
              Field pattern: Field 1 only
            bCopyProtect                        0
            bVariableSize                     1
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                 17 (FRAME_FRAME_BASED)
            bFrameIndex                         1
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           1280
            wHeight                           720
            dwMinBitRate                     1000
            dwMaxBitRate                 10000000
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  0
            dwBytesPerLine                      0
            dwMinFrameInterval             333333
            dwMaxFrameInterval           10000000
            dwFrameIntervalStep                 1
          VideoStreaming Interface Descriptor:
            bLength                             6
            bDescriptorType                    36
            bDescriptorSubtype                 13 (COLORFORMAT)
            bColorPrimaries                     1 (BT.709,sRGB)
            bTransferCharacteristics            1 (BT.709)
            bMatrixCoefficients                 4 (SMPTE 170M (BT.601))
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x82  EP 2 IN
            bmAttributes            2
              Transfer Type            Bulk
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0200  1x 512 bytes
            bInterval               0
        Interface Association:
          bLength                 8
          bDescriptorType        11
          bFirstInterface         2
          bInterfaceCount         2
          bFunctionClass         14 Video
          bFunctionSubClass       3 Video Interface Collection
          bFunctionProtocol       0
          iFunction               2 MAX64380
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        2
          bAlternateSetting       0
          bNumEndpoints           1
          bInterfaceClass        14 Video
          bInterfaceSubClass      1 Video Control
          bInterfaceProtocol      0
          iInterface              2 MAX64380
          VideoControl Interface Descriptor:
            bLength                13
            bDescriptorType        36
            bDescriptorSubtype      1 (HEADER)
            bcdUVC               1.10
            wTotalLength       0x006f
            dwClockFrequency        6.000000MHz
            bInCollection           1
            baInterfaceNr( 0)       3
          VideoControl Interface Descriptor:
            bLength                18
            bDescriptorType        36
            bDescriptorSubtype      2 (INPUT_TERMINAL)
            bTerminalID             1
            wTerminalType      0x0201 Camera Sensor
            bAssocTerminal          0
            iTerminal               0
            wObjectiveFocalLengthMin      0
            wObjectiveFocalLengthMax      0
            wOcularFocalLength            0
            bControlSize                  3
            bmControls           0x00000800
              PanTilt (Absolute)
          VideoControl Interface Descriptor:
            bLength                13
            bDescriptorType        36
            bDescriptorSubtype      5 (PROCESSING_UNIT)
            bUnitID                 4
            bSourceID               1
            wMaxMultiplier        100
            bControlSize            3
            bmControls     0x0000463f
              Brightness
              Contrast
              Hue
              Saturation
              Sharpness
              Gamma
              Gain
              Power Line Frequency
              Digital Multiplier
            iProcessing             0
            bmVideoStandards     0x02
              NTSC - 525/60
          VideoControl Interface Descriptor:
            bLength                29
            bDescriptorType        36
            bDescriptorSubtype      6 (EXTENSION_UNIT)
            bUnitID                 5
            guidExtensionCode         {ba2b92d9-26f2-4294-42ae-e4eb4d68dd06}
            bNumControls           14
            bNrInPins               1
            baSourceID( 0)          4
            bControlSize            4
            bmControls( 0)       0xff
            bmControls( 1)       0x2f
            bmControls( 2)       0x00
            bmControls( 3)       0x00
            iExtension              0
          VideoControl Interface Descriptor:
            bLength                29
            bDescriptorType        36
            bDescriptorSubtype      6 (EXTENSION_UNIT)
            bUnitID                 6
            guidExtensionCode         {df5dcd12-7d5f-4bba-bb6d-4b625add5272}
            bNumControls           19
            bNrInPins               1
            baSourceID( 0)          4
            bControlSize            4
            bmControls( 0)       0xff
            bmControls( 1)       0x87
            bmControls( 2)       0x07
            bmControls( 3)       0x00
            iExtension              0
          VideoControl Interface Descriptor:
            bLength                 9
            bDescriptorType        36
            bDescriptorSubtype      3 (OUTPUT_TERMINAL)
            bTerminalID             2
            wTerminalType      0x0101 USB Streaming
            bAssocTerminal          0
            bSourceID               5
            iTerminal               0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x85  EP 5 IN
            bmAttributes            3
              Transfer Type            Interrupt
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0008  1x 8 bytes
            bInterval               9
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        3
          bAlternateSetting       0
          bNumEndpoints           1
          bInterfaceClass        14 Video
          bInterfaceSubClass      2 Video Streaming
          bInterfaceProtocol      0
          iInterface              0
          VideoStreaming Interface Descriptor:
            bLength                            17
            bDescriptorType                    36
            bDescriptorSubtype                  1 (INPUT_HEADER)
            bNumFormats                         4
            wTotalLength                   0x028a
            bEndpointAddress                 0x83  EP 3 IN
            bmInfo                              0
            bTerminalLink                       2
            bStillCaptureMethod                 0
            bTriggerSupport                     0
            bTriggerUsage                       0
            bControlSize                        1
            bmaControls( 0)                     0
            bmaControls( 1)                     4
            bmaControls( 2)                     0
            bmaControls( 3)                     0
          VideoStreaming Interface Descriptor:
            bLength                            27
            bDescriptorType                    36
            bDescriptorSubtype                  4 (FORMAT_UNCOMPRESSED)
            bFormatIndex                        1
            bNumFrameDescriptors                6
            guidFormat                            {32595559-0000-0010-8000-00aa00389b71}
            bBitsPerPixel                      16
            bDefaultFrameIndex                  1
            bAspectRatioX                       0
            bAspectRatioY                       0
            bmInterlaceFlags                 0x00
              Interlaced stream or variable: No
              Fields per frame: 2 fields
              Field 1 first: No
              Field pattern: Field 1 only
            bCopyProtect                        0
          VideoStreaming Interface Descriptor:
            bLength                            46
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         1
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            160
            wHeight                           120
            dwMinBitRate                  3072000
            dwMaxBitRate                  9216000
            dwMaxVideoFrameBufferSize       38400
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  5
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            400000
            dwFrameInterval( 2)            500000
            dwFrameInterval( 3)            666666
            dwFrameInterval( 4)           1000000
          VideoStreaming Interface Descriptor:
            bLength                            46
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         2
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            320
            wHeight                           240
            dwMinBitRate                 12288000
            dwMaxBitRate                 36864000
            dwMaxVideoFrameBufferSize      153600
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  5
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            400000
            dwFrameInterval( 2)            500000
            dwFrameInterval( 3)            666666
            dwFrameInterval( 4)           1000000
          VideoStreaming Interface Descriptor:
            bLength                            46
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         3
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            432
            wHeight                           240
            dwMinBitRate                 16588800
            dwMaxBitRate                 49766400
            dwMaxVideoFrameBufferSize      207360
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  5
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            400000
            dwFrameInterval( 2)            500000
            dwFrameInterval( 3)            666666
            dwFrameInterval( 4)           1000000
          VideoStreaming Interface Descriptor:
            bLength                            46
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         4
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            624
            wHeight                           352
            dwMinBitRate                 35143680
            dwMaxBitRate                105431040
            dwMaxVideoFrameBufferSize      439296
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  5
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            400000
            dwFrameInterval( 2)            500000
            dwFrameInterval( 3)            666666
            dwFrameInterval( 4)           1000000
          VideoStreaming Interface Descriptor:
            bLength                            46
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         5
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            640
            wHeight                           480
            dwMinBitRate                 49152000
            dwMaxBitRate                147456000
            dwMaxVideoFrameBufferSize      614400
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  5
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            400000
            dwFrameInterval( 2)            500000
            dwFrameInterval( 3)            666666
            dwFrameInterval( 4)           1000000
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         6
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           1280
            wHeight                           720
            dwMinBitRate                 58982400
            dwMaxBitRate                 58982400
            dwMaxVideoFrameBufferSize     1843200
            dwDefaultFrameInterval        2500000
            bFrameIntervalType                  1
            dwFrameInterval( 0)           2500000
          VideoStreaming Interface Descriptor:
            bLength                             6
            bDescriptorType                    36
            bDescriptorSubtype                 13 (COLORFORMAT)
            bColorPrimaries                     1 (BT.709,sRGB)
            bTransferCharacteristics            1 (BT.709)
            bMatrixCoefficients                 4 (SMPTE 170M (BT.601))
          VideoStreaming Interface Descriptor:
            bLength                            11
            bDescriptorType                    36
            bDescriptorSubtype                  6 (FORMAT_MJPEG)
            bFormatIndex                        2
            bNumFrameDescriptors                6
            bFlags                              0
              Fixed-size samples: No
            bDefaultFrameIndex                  1
            bAspectRatioX                       0
            bAspectRatioY                       0
            bmInterlaceFlags                 0x00
              Interlaced stream or variable: No
              Fields per frame: 1 fields
              Field 1 first: No
              Field pattern: Field 1 only
            bCopyProtect                        0
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         1
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            640
            wHeight                           480
            dwMinBitRate                  4194304
            dwMaxBitRate                  5767168
            dwMaxVideoFrameBufferSize       65536
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  0
            dwMinFrameInterval             333333
            dwMaxFrameInterval           10000000
            dwFrameIntervalStep                 1
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         2
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            624
            wHeight                           352
            dwMinBitRate                  4194304
            dwMaxBitRate                  5767168
            dwMaxVideoFrameBufferSize       65536
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  0
            dwMinFrameInterval             333333
            dwMaxFrameInterval           10000000
            dwFrameIntervalStep                 1
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         3
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            320
            wHeight                           240
            dwMinBitRate                  1048576
            dwMaxBitRate                  1441792
            dwMaxVideoFrameBufferSize       65536
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  0
            dwMinFrameInterval             333333
            dwMaxFrameInterval           10000000
            dwFrameIntervalStep                 1
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         4
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            432
            wHeight                           240
            dwMinBitRate                  1048576
            dwMaxBitRate                  1441792
            dwMaxVideoFrameBufferSize       65536
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  0
            dwMinFrameInterval             333333
            dwMaxFrameInterval           10000000
            dwFrameIntervalStep                 1
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         5
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           1280
            wHeight                           720
            dwMinBitRate                  6291456
            dwMaxBitRate                  8388608
            dwMaxVideoFrameBufferSize       98304
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  0
            dwMinFrameInterval             333333
            dwMaxFrameInterval           10000000
            dwFrameIntervalStep                 1
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         6
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            960
            wHeight                           720
            dwMinBitRate                  6291456
            dwMaxBitRate                  8388608
            dwMaxVideoFrameBufferSize       98304
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  0
            dwMinFrameInterval             333333
            dwMaxFrameInterval           10000000
            dwFrameIntervalStep                 1
          VideoStreaming Interface Descriptor:
            bLength                             6
            bDescriptorType                    36
            bDescriptorSubtype                 13 (COLORFORMAT)
            bColorPrimaries                     1 (BT.709,sRGB)
            bTransferCharacteristics            1 (BT.709)
            bMatrixCoefficients                 4 (SMPTE 170M (BT.601))
          VideoStreaming Interface Descriptor:
            bLength                            23
            bDescriptorType                    36
            bDescriptorSubtype                 10 (FORMAT_MPEG2TS)
            bFormatIndex                        3
            bDataOffset                         0
            bPacketLength                     188
            bStrideLength                     188
            guidStrideFormat                      {00000000-0000-0000-0000-000000000000}
          VideoStreaming Interface Descriptor:
            bLength                            28
            bDescriptorType                    36
            bDescriptorSubtype                 16 (FORMAT_FRAME_BASED)
            bFormatIndex                        4
            bNumFrameDescriptors                1
            guidFormat                            {34363248-0000-0010-8000-00aa00389b71}
            bBitsPerPixel                      12
            bDefaultFrameIndex                  1
            bAspectRatioX                      16
            bAspectRatioY                       9
            bmInterlaceFlags                 0x00
              Interlaced stream or variable: No
              Fields per frame: 2 fields
              Field 1 first: No
              Field pattern: Field 1 only
            bCopyProtect                        0
            bVariableSize                     1
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                 17 (FRAME_FRAME_BASED)
            bFrameIndex                         1
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           1280
            wHeight                           720
            dwMinBitRate                     1000
            dwMaxBitRate                 10000000
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  0
            dwBytesPerLine                      0
            dwMinFrameInterval             333333
            dwMaxFrameInterval           10000000
            dwFrameIntervalStep                 1
          VideoStreaming Interface Descriptor:
            bLength                             6
            bDescriptorType                    36
            bDescriptorSubtype                 13 (COLORFORMAT)
            bColorPrimaries                     1 (BT.709,sRGB)
            bTransferCharacteristics            1 (BT.709)
            bMatrixCoefficients                 4 (SMPTE 170M (BT.601))
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x83  EP 3 IN
            bmAttributes            2
              Transfer Type            Bulk
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0200  1x 512 bytes
            bInterval               0
        Interface Association:
          bLength                 8
          bDescriptorType        11
          bFirstInterface         4
          bInterfaceCount         2
          bFunctionClass          1 Audio
          bFunctionSubClass       2 Streaming
          bFunctionProtocol       0
          iFunction               2 MAX64380
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        4
          bAlternateSetting       0
          bNumEndpoints           0
          bInterfaceClass         1 Audio
          bInterfaceSubClass      1 Control Device
          bInterfaceProtocol      0
          iInterface              0
          AudioControl Interface Descriptor:
            bLength                 9
            bDescriptorType        36
            bDescriptorSubtype      1 (HEADER)
            bcdADC               1.00
            wTotalLength       0x0027
            bInCollection           1
            baInterfaceNr(0)        5
          AudioControl Interface Descriptor:
            bLength                12
            bDescriptorType        36
            bDescriptorSubtype      2 (INPUT_TERMINAL)
            bTerminalID             1
            wTerminalType      0x0201 Microphone
            bAssocTerminal          0
            bNrChannels             2
            wChannelConfig     0x0003
              Left Front (L)
              Right Front (R)
            iChannelNames           0
            iTerminal               0
          AudioControl Interface Descriptor:
            bLength                 9
            bDescriptorType        36
            bDescriptorSubtype      6 (FEATURE_UNIT)
            bUnitID                 2
            bSourceID               1
            bControlSize            1
            bmaControls(0)       0x01
              Mute Control
            bmaControls(1)       0x02
              Volume Control
            iFeature                0
          AudioControl Interface Descriptor:
            bLength                 9
            bDescriptorType        36
            bDescriptorSubtype      3 (OUTPUT_TERMINAL)
            bTerminalID             3
            wTerminalType      0x0101 USB Streaming
            bAssocTerminal          0
            bSourceID               2
            iTerminal               0
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        5
          bAlternateSetting       0
          bNumEndpoints           0
          bInterfaceClass         1 Audio
          bInterfaceSubClass      2 Streaming
          bInterfaceProtocol      0
          iInterface              0
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        5
          bAlternateSetting       1
          bNumEndpoints           1
          bInterfaceClass         1 Audio
          bInterfaceSubClass      2 Streaming
          bInterfaceProtocol      0
          iInterface              0
          AudioStreaming Interface Descriptor:
            bLength                 7
            bDescriptorType        36
            bDescriptorSubtype      1 (AS_GENERAL)
            bTerminalLink           3
            bDelay                  1 frames
            wFormatTag         0x0001 PCM
          AudioStreaming Interface Descriptor:
            bLength                11
            bDescriptorType        36
            bDescriptorSubtype      2 (FORMAT_TYPE)
            bFormatType             1 (FORMAT_TYPE_I)
            bNrChannels             2
            bSubframeSize           2
            bBitResolution         16
            bSamFreqType            1 Discrete
            tSamFreq[ 0]         8000
          Endpoint Descriptor:
            bLength                 9
            bDescriptorType         5
            bEndpointAddress     0x81  EP 1 IN
            bmAttributes            5
              Transfer Type            Isochronous
              Synch Type               Asynchronous
              Usage Type               Data
            wMaxPacketSize     0x00c0  1x 192 bytes
            bInterval               4
            bRefresh                0
            bSynchAddress           0
            AudioStreaming Endpoint Descriptor:
              bLength                 7
              bDescriptorType        37
              bDescriptorSubtype      1 (EP_GENERAL)
              bmAttributes         0x00
              bLockDelayUnits         0 Undefined
              wLockDelay         0x0000
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        5
          bAlternateSetting       2
          bNumEndpoints           1
          bInterfaceClass         1 Audio
          bInterfaceSubClass      2 Streaming
          bInterfaceProtocol      0
          iInterface              0
          AudioStreaming Interface Descriptor:
            bLength                 7
            bDescriptorType        36
            bDescriptorSubtype      1 (AS_GENERAL)
            bTerminalLink           3
            bDelay                  1 frames
            wFormatTag         0x0001 PCM
          AudioStreaming Interface Descriptor:
            bLength                11
            bDescriptorType        36
            bDescriptorSubtype      2 (FORMAT_TYPE)
            bFormatType             1 (FORMAT_TYPE_I)
            bNrChannels             2
            bSubframeSize           2
            bBitResolution         16
            bSamFreqType            1 Discrete
            tSamFreq[ 0]        16000
          Endpoint Descriptor:
            bLength                 9
            bDescriptorType         5
            bEndpointAddress     0x81  EP 1 IN
            bmAttributes            5
              Transfer Type            Isochronous
              Synch Type               Asynchronous
              Usage Type               Data
            wMaxPacketSize     0x00c0  1x 192 bytes
            bInterval               4
            bRefresh                0
            bSynchAddress           0
            AudioStreaming Endpoint Descriptor:
              bLength                 7
              bDescriptorType        37
              bDescriptorSubtype      1 (EP_GENERAL)
              bmAttributes         0x00
              bLockDelayUnits         0 Undefined
              wLockDelay         0x0000
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        5
          bAlternateSetting       3
          bNumEndpoints           1
          bInterfaceClass         1 Audio
          bInterfaceSubClass      2 Streaming
          bInterfaceProtocol      0
          iInterface              0
          AudioStreaming Interface Descriptor:
            bLength                 7
            bDescriptorType        36
            bDescriptorSubtype      1 (AS_GENERAL)
            bTerminalLink           3
            bDelay                  1 frames
            wFormatTag         0x0001 PCM
          AudioStreaming Interface Descriptor:
            bLength                11
            bDescriptorType        36
            bDescriptorSubtype      2 (FORMAT_TYPE)
            bFormatType             1 (FORMAT_TYPE_I)
            bNrChannels             2
            bSubframeSize           2
            bBitResolution         16
            bSamFreqType            1 Discrete
            tSamFreq[ 0]        24000
          Endpoint Descriptor:
            bLength                 9
            bDescriptorType         5
            bEndpointAddress     0x81  EP 1 IN
            bmAttributes            5
              Transfer Type            Isochronous
              Synch Type               Asynchronous
              Usage Type               Data
            wMaxPacketSize     0x00c0  1x 192 bytes
            bInterval               4
            bRefresh                0
            bSynchAddress           0
            AudioStreaming Endpoint Descriptor:
              bLength                 7
              bDescriptorType        37
              bDescriptorSubtype      1 (EP_GENERAL)
              bmAttributes         0x00
              bLockDelayUnits         0 Undefined
              wLockDelay         0x0000
    Device Qualifier (for other device speed):
      bLength                10
      bDescriptorType         6
      bcdUSB               2.00
      bDeviceClass          239 Miscellaneous Device
      bDeviceSubClass         2
      bDeviceProtocol         1 Interface Association
      bMaxPacketSize0        64
      bNumConfigurations      2
    Device Status:     0x0000
      (Bus Powered)

### VG-STC3000
 * 1280x720/30fps
 * Alpha Imaging Tech. AIT8453 (firmware begins with "AIT842SF" string)

lsusb (no firmware loaded):

    Bus 002 Device 007: ID 04e8:205c Samsung Electronics Co., Ltd USB2.0 UVC HQ WebCam
    Device Descriptor:
      bLength                18
      bDescriptorType         1
      bcdUSB               2.00
      bDeviceClass          239 Miscellaneous Device
      bDeviceSubClass         2
      bDeviceProtocol         1 Interface Association
      bMaxPacketSize0        64
      idVendor           0x04e8 Samsung Electronics Co., Ltd
      idProduct          0x205c
      bcdDevice            7.30
      iManufacturer           1 Alpha Imaging Tech. Corp.
      iProduct                2 USB2.0 UVC HQ WebCam
      iSerial                 0
      bNumConfigurations      1
      Configuration Descriptor:
        bLength                 9
        bDescriptorType         2
        wTotalLength       0x01f7
        bNumInterfaces          4
        bConfigurationValue     1
        iConfiguration          0
        bmAttributes         0x80
          (Bus Powered)
        MaxPower              500mA
        Interface Association:
          bLength                 8
          bDescriptorType        11
          bFirstInterface         0
          bInterfaceCount         2
          bFunctionClass         14 Video
          bFunctionSubClass       3 Video Interface Collection
          bFunctionProtocol       0
          iFunction               0
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        0
          bAlternateSetting       0
          bNumEndpoints           1
          bInterfaceClass        14 Video
          bInterfaceSubClass      1 Video Control
          bInterfaceProtocol      0
          iInterface              0
          VideoControl Interface Descriptor:
            bLength                13
            bDescriptorType        36
            bDescriptorSubtype      1 (HEADER)
            bcdUVC               1.00
            wTotalLength       0x0084
            dwClockFrequency      144.000000MHz
            bInCollection           1
            baInterfaceNr( 0)       1
          VideoControl Interface Descriptor:
            bLength                 9
            bDescriptorType        36
            bDescriptorSubtype      3 (OUTPUT_TERMINAL)
            bTerminalID             4
            wTerminalType      0x0101 USB Streaming
            bAssocTerminal          0
            bSourceID               2
            iTerminal               0
          VideoControl Interface Descriptor:
            bLength                27
            bDescriptorType        36
            bDescriptorSubtype      6 (EXTENSION_UNIT)
            bUnitID                 2
            guidExtensionCode         {2c49d16a-32b8-4485-3ea8-643a152362f2}
            bNumControl             6
            bNrPins                 1
            baSourceID( 0)          6
            bControlSize            2
            bmControls( 0)       0x00
            bmControls( 1)       0x00
            iExtension              0
          VideoControl Interface Descriptor:
            bLength                27
            bDescriptorType        36
            bDescriptorSubtype      6 (EXTENSION_UNIT)
            bUnitID                 6
            guidExtensionCode         {23e49ed0-1178-4f31-ae52-d2fb8a8d3b48}
            bNumControl             5
            bNrPins                 1
            baSourceID( 0)          5
            bControlSize            2
            bmControls( 0)       0x1f
            bmControls( 1)       0x00
            iExtension              0
          VideoControl Interface Descriptor:
            bLength                27
            bDescriptorType        36
            bDescriptorSubtype      6 (EXTENSION_UNIT)
            bUnitID                 5
            guidExtensionCode         {b42153bd-35d6-45ca-b203-4e0149b301bc}
            bNumControl            16
            bNrPins                 1
            baSourceID( 0)          3
            bControlSize            2
            bmControls( 0)       0x00
            bmControls( 1)       0x00
            iExtension              0
          VideoControl Interface Descriptor:
            bLength                18
            bDescriptorType        36
            bDescriptorSubtype      2 (INPUT_TERMINAL)
            bTerminalID             1
            wTerminalType      0x0201 Camera Sensor
            bAssocTerminal          0
            iTerminal               0
            wObjectiveFocalLengthMin      0
            wObjectiveFocalLengthMax      0
            wOcularFocalLength            0
            bControlSize                  3
            bmControls           0x00000000
          VideoControl Interface Descriptor:
            bLength                11
            bDescriptorType        36
            bDescriptorSubtype      5 (PROCESSING_UNIT)
          Warning: Descriptor too short
            bUnitID                 3
            bSourceID               1
            wMaxMultiplier          0
            bControlSize            2
            bmControls     0x00000000
            iProcessing             0
            bmVideoStandards     0x00
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x83  EP 3 IN
            bmAttributes            3
              Transfer Type            Interrupt
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0040  1x 64 bytes
            bInterval               8
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        1
          bAlternateSetting       0
          bNumEndpoints           0
          bInterfaceClass        14 Video
          bInterfaceSubClass      2 Video Streaming
          bInterfaceProtocol      0
          iInterface              0
          VideoStreaming Interface Descriptor:
            bLength                            14
            bDescriptorType                    36
            bDescriptorSubtype                  1 (INPUT_HEADER)
            bNumFormats                         1
            wTotalLength                   0x011d
            bEndPointAddress                  129
            bmInfo                              0
            bTerminalLink                       4
            bStillCaptureMethod                 2
            bTriggerSupport                     1
            bTriggerUsage                       0
            bControlSize                        1
            bmaControls( 0)                     0
          VideoStreaming Interface Descriptor:
            bLength                            27
            bDescriptorType                    36
            bDescriptorSubtype                  4 (FORMAT_UNCOMPRESSED)
            bFormatIndex                        1
            bNumFrameDescriptors                7
            guidFormat                            {32595559-0000-0010-8000-00aa00389b71}
            bBitsPerPixel                      16
            bDefaultFrameIndex                  1
            bAspectRatioX                       0
            bAspectRatioY                       0
            bmInterlaceFlags                 0x00
              Interlaced stream or variable: No
              Fields per frame: 2 fields
              Field 1 first: No
              Field pattern: Field 1 only
            bCopyProtect                        0
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         1
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            640
            wHeight                           480
            dwMinBitRate                147456000
            dwMaxBitRate                147456000
            dwMaxVideoFrameBufferSize      614400
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         2
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            848
            wHeight                           480
            dwMinBitRate                195379200
            dwMaxBitRate                195379200
            dwMaxVideoFrameBufferSize      814080
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         3
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            320
            wHeight                           240
            dwMinBitRate                 36864000
            dwMaxBitRate                 36864000
            dwMaxVideoFrameBufferSize      153600
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         4
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            400
            wHeight                           296
            dwMinBitRate                 56832000
            dwMaxBitRate                 56832000
            dwMaxVideoFrameBufferSize      236800
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         5
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            384
            wHeight                           216
            dwMinBitRate                 39813120
            dwMaxBitRate                 39813120
            dwMaxVideoFrameBufferSize      165888
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         6
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            352
            wHeight                           288
            dwMinBitRate                 48660480
            dwMaxBitRate                 48660480
            dwMaxVideoFrameBufferSize      202752
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         7
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           1280
            wHeight                           720
            dwMinBitRate                147456000
            dwMaxBitRate                147456000
            dwMaxVideoFrameBufferSize     1843200
            dwDefaultFrameInterval        1000000
            bFrameIntervalType                  1
            dwFrameInterval( 0)           1000000
          VideoStreaming Interface Descriptor:
            bLength                            19
            bDescriptorType                    36
            bDescriptorSubtype                  3 (STILL_IMAGE_FRAME)
            bEndpointAddress                    0
            bNumImageSizePatterns               3
            wWidth( 0)                        640
            wHeight( 0)                       480
            wWidth( 1)                        320
            wHeight( 1)                       240
            wWidth( 2)                        160
            wHeight( 2)                       120
            bNumCompressionPatterns             1
            bCompression( 0)                    3
          VideoStreaming Interface Descriptor:
            bLength                             6
            bDescriptorType                    36
            bDescriptorSubtype                 13 (COLORFORMAT)
            bColorPrimaries                     1 (BT.709,sRGB)
            bTransferCharacteristics            1 (BT.709)
            bMatrixCoefficients                 4 (SMPTE 170M (BT.601))
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        1
          bAlternateSetting       1
          bNumEndpoints           1
          bInterfaceClass        14 Video
          bInterfaceSubClass      2 Video Streaming
          bInterfaceProtocol      0
          iInterface              0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x81  EP 1 IN
            bmAttributes            5
              Transfer Type            Isochronous
              Synch Type               Asynchronous
              Usage Type               Data
            wMaxPacketSize     0x13f8  3x 1016 bytes
            bInterval               1
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        2
          bAlternateSetting       0
          bNumEndpoints           2
          bInterfaceClass       255 Vendor Specific Class
          bInterfaceSubClass    255 Vendor Specific Subclass
          bInterfaceProtocol    255 Vendor Specific Protocol
          iInterface              0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x81  EP 1 IN
            bmAttributes            2
              Transfer Type            Bulk
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0200  1x 512 bytes
            bInterval               0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x01  EP 1 OUT
            bmAttributes            2
              Transfer Type            Bulk
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0200  1x 512 bytes
            bInterval               0
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        3
          bAlternateSetting       0
          bNumEndpoints           0
          bInterfaceClass       255 Vendor Specific Class
          bInterfaceSubClass    255 Vendor Specific Subclass
          bInterfaceProtocol    255 Vendor Specific Protocol
          iInterface              0
    Device Qualifier (for other device speed):
      bLength                10
      bDescriptorType         6
      bcdUSB               2.00
      bDeviceClass          239 Miscellaneous Device
      bDeviceSubClass         2
      bDeviceProtocol         1 Interface Association
      bMaxPacketSize0        64
      bNumConfigurations      1
    Device Status:     0x0000
      (Bus Powered)

### VG-STC4000
 * 1920x1080/30fps
 * Alpha Imaging Tech. AIT???? (firmware begins with "AIT842SF" string)

lsusb (no firmware loaded):

    Bus 001 Device 002: ID 04e8:2061 Samsung Electronics Co., Ltd USB2.0 UVC HQ WebCam
    Device Descriptor:
      bLength                18
      bDescriptorType         1
      bcdUSB               2.00
      bDeviceClass          239 Miscellaneous Device
      bDeviceSubClass         2
      bDeviceProtocol         1 Interface Association
      bMaxPacketSize0        64
      idVendor           0x04e8 Samsung Electronics Co., Ltd
      idProduct          0x2061
      bcdDevice            7.30
      iManufacturer           1 Alpha Imaging Tech. Corp.
      iProduct                2 USB2.0 UVC HQ WebCam
      iSerial                 0
      bNumConfigurations      1
      Configuration Descriptor:
        bLength                 9
        bDescriptorType         2
        wTotalLength       0x01f7
        bNumInterfaces          4
        bConfigurationValue     1
        iConfiguration          0
        bmAttributes         0x80
          (Bus Powered)
        MaxPower              500mA
        Interface Association:
          bLength                 8
          bDescriptorType        11
          bFirstInterface         0
          bInterfaceCount         2
          bFunctionClass         14 Video
          bFunctionSubClass       3 Video Interface Collection
          bFunctionProtocol       0
          iFunction               0
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        0
          bAlternateSetting       0
          bNumEndpoints           1
          bInterfaceClass        14 Video
          bInterfaceSubClass      1 Video Control
          bInterfaceProtocol      0
          iInterface              0
          VideoControl Interface Descriptor:
            bLength                13
            bDescriptorType        36
            bDescriptorSubtype      1 (HEADER)
            bcdUVC               1.00
            wTotalLength       0x0084
            dwClockFrequency      144.000000MHz
            bInCollection           1
            baInterfaceNr( 0)       1
          VideoControl Interface Descriptor:
            bLength                 9
            bDescriptorType        36
            bDescriptorSubtype      3 (OUTPUT_TERMINAL)
            bTerminalID             4
            wTerminalType      0x0101 USB Streaming
            bAssocTerminal          0
            bSourceID               2
            iTerminal               0
          VideoControl Interface Descriptor:
            bLength                27
            bDescriptorType        36
            bDescriptorSubtype      6 (EXTENSION_UNIT)
            bUnitID                 2
            guidExtensionCode         {2c49d16a-32b8-4485-3ea8-643a152362f2}
            bNumControls            6
            bNrInPins               1
            baSourceID( 0)          6
            bControlSize            2
            bmControls( 0)       0x00
            bmControls( 1)       0x00
            iExtension              0
          VideoControl Interface Descriptor:
            bLength                27
            bDescriptorType        36
            bDescriptorSubtype      6 (EXTENSION_UNIT)
            bUnitID                 6
            guidExtensionCode         {23e49ed0-1178-4f31-ae52-d2fb8a8d3b48}
            bNumControls            5
            bNrInPins               1
            baSourceID( 0)          5
            bControlSize            2
            bmControls( 0)       0x1f
            bmControls( 1)       0x00
            iExtension              0
          VideoControl Interface Descriptor:
            bLength                27
            bDescriptorType        36
            bDescriptorSubtype      6 (EXTENSION_UNIT)
            bUnitID                 5
            guidExtensionCode         {b42153bd-35d6-45ca-b203-4e0149b301bc}
            bNumControls           16
            bNrInPins               1
            baSourceID( 0)          3
            bControlSize            2
            bmControls( 0)       0x00
            bmControls( 1)       0x00
            iExtension              0
          VideoControl Interface Descriptor:
            bLength                18
            bDescriptorType        36
            bDescriptorSubtype      2 (INPUT_TERMINAL)
            bTerminalID             1
            wTerminalType      0x0201 Camera Sensor
            bAssocTerminal          0
            iTerminal               0
            wObjectiveFocalLengthMin      0
            wObjectiveFocalLengthMax      0
            wOcularFocalLength            0
            bControlSize                  3
            bmControls           0x00000000
          VideoControl Interface Descriptor:
            bLength                11
            bDescriptorType        36
            bDescriptorSubtype      5 (PROCESSING_UNIT)
          Warning: Descriptor too short
            bUnitID                 3
            bSourceID               1
            wMaxMultiplier          0
            bControlSize            2
            bmControls     0x00000000
            iProcessing             0
            bmVideoStandards     0x4f
              None
              NTSC - 525/60
              PAL - 625/50
              SECAM - 625/50
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x83  EP 3 IN
            bmAttributes            3
              Transfer Type            Interrupt
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0040  1x 64 bytes
            bInterval               8
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        1
          bAlternateSetting       0
          bNumEndpoints           0
          bInterfaceClass        14 Video
          bInterfaceSubClass      2 Video Streaming
          bInterfaceProtocol      0
          iInterface              0
          VideoStreaming Interface Descriptor:
            bLength                            14
            bDescriptorType                    36
            bDescriptorSubtype                  1 (INPUT_HEADER)
            bNumFormats                         1
            wTotalLength                   0x011d
            bEndpointAddress                 0x81  EP 1 IN
            bmInfo                              0
            bTerminalLink                       4
            bStillCaptureMethod                 2
            bTriggerSupport                     1
            bTriggerUsage                       0
            bControlSize                        1
            bmaControls( 0)                     0
          VideoStreaming Interface Descriptor:
            bLength                            27
            bDescriptorType                    36
            bDescriptorSubtype                  4 (FORMAT_UNCOMPRESSED)
            bFormatIndex                        1
            bNumFrameDescriptors                7
            guidFormat                            {32595559-0000-0010-8000-00aa00389b71}
            bBitsPerPixel                      16
            bDefaultFrameIndex                  1
            bAspectRatioX                       0
            bAspectRatioY                       0
            bmInterlaceFlags                 0x00
              Interlaced stream or variable: No
              Fields per frame: 2 fields
              Field 1 first: No
              Field pattern: Field 1 only
            bCopyProtect                        0
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         1
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            640
            wHeight                           480
            dwMinBitRate                147456000
            dwMaxBitRate                147456000
            dwMaxVideoFrameBufferSize      614400
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         2
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            848
            wHeight                           480
            dwMinBitRate                195379200
            dwMaxBitRate                195379200
            dwMaxVideoFrameBufferSize      814080
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         3
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            320
            wHeight                           240
            dwMinBitRate                 36864000
            dwMaxBitRate                 36864000
            dwMaxVideoFrameBufferSize      153600
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         4
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            400
            wHeight                           296
            dwMinBitRate                 56832000
            dwMaxBitRate                 56832000
            dwMaxVideoFrameBufferSize      236800
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         5
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            384
            wHeight                           216
            dwMinBitRate                 39813120
            dwMaxBitRate                 39813120
            dwMaxVideoFrameBufferSize      165888
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         6
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            352
            wHeight                           288
            dwMinBitRate                 48660480
            dwMaxBitRate                 48660480
            dwMaxVideoFrameBufferSize      202752
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         7
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           1280
            wHeight                           720
            dwMinBitRate                147456000
            dwMaxBitRate                147456000
            dwMaxVideoFrameBufferSize     1843200
            dwDefaultFrameInterval        1000000
            bFrameIntervalType                  1
            dwFrameInterval( 0)           1000000
          VideoStreaming Interface Descriptor:
            bLength                            19
            bDescriptorType                    36
            bDescriptorSubtype                  3 (STILL_IMAGE_FRAME)
            bEndpointAddress                 0x00  EP 0 OUT
            bNumImageSizePatterns               3
            wWidth( 0)                        640
            wHeight( 0)                       480
            wWidth( 1)                        320
            wHeight( 1)                       240
            wWidth( 2)                        160
            wHeight( 2)                       120
            bNumCompressionPatterns             1
            bCompression( 0)                    3
          VideoStreaming Interface Descriptor:
            bLength                             6
            bDescriptorType                    36
            bDescriptorSubtype                 13 (COLORFORMAT)
            bColorPrimaries                     1 (BT.709,sRGB)
            bTransferCharacteristics            1 (BT.709)
            bMatrixCoefficients                 4 (SMPTE 170M (BT.601))
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        1
          bAlternateSetting       1
          bNumEndpoints           1
          bInterfaceClass        14 Video
          bInterfaceSubClass      2 Video Streaming
          bInterfaceProtocol      0
          iInterface              0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x81  EP 1 IN
            bmAttributes            5
              Transfer Type            Isochronous
              Synch Type               Asynchronous
              Usage Type               Data
            wMaxPacketSize     0x13f8  3x 1016 bytes
            bInterval               1
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        2
          bAlternateSetting       0
          bNumEndpoints           2
          bInterfaceClass       255 Vendor Specific Class
          bInterfaceSubClass    255 Vendor Specific Subclass
          bInterfaceProtocol    255 Vendor Specific Protocol
          iInterface              0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x81  EP 1 IN
            bmAttributes            2
              Transfer Type            Bulk
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0200  1x 512 bytes
            bInterval               0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x01  EP 1 OUT
            bmAttributes            2
              Transfer Type            Bulk
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0200  1x 512 bytes
            bInterval               0
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        3
          bAlternateSetting       0
          bNumEndpoints           0
          bInterfaceClass       255 Vendor Specific Class
          bInterfaceSubClass    255 Vendor Specific Subclass
          bInterfaceProtocol    255 Vendor Specific Protocol
          iInterface              0
    Device Qualifier (for other device speed):
      bLength                10
      bDescriptorType         6
      bcdUSB               2.00
      bDeviceClass          239 Miscellaneous Device
      bDeviceSubClass         2
      bDeviceProtocol         1 Interface Association
      bMaxPacketSize0        64
      bNumConfigurations      1
    Device Status:     0x0000
      (Bus Powered)

lsusb (firmware loaded):

    Bus 001 Device 003: ID 04e8:2062 Samsung Electronics Co., Ltd SAMSUNG VD USB HDCAM
    Device Descriptor:
      bLength                18
      bDescriptorType         1
      bcdUSB               2.00
      bDeviceClass          239 Miscellaneous Device
      bDeviceSubClass         2
      bDeviceProtocol         1 Interface Association
      bMaxPacketSize0        64
      idVendor           0x04e8 Samsung Electronics Co., Ltd
      idProduct          0x2062
      bcdDevice            7.30
      iManufacturer           1 Alpha Imaging Tech. Corp.
      iProduct                2 SAMSUNG VD USB HDCAM
      iSerial                 0
      bNumConfigurations      1
      Configuration Descriptor:
        bLength                 9
        bDescriptorType         2
        wTotalLength       0x044d
        bNumInterfaces          6
        bConfigurationValue     1
        iConfiguration          0
        bmAttributes         0x80
          (Bus Powered)
        MaxPower              500mA
        Interface Association:
          bLength                 8
          bDescriptorType        11
          bFirstInterface         0
          bInterfaceCount         2
          bFunctionClass         14 Video
          bFunctionSubClass       3 Video Interface Collection
          bFunctionProtocol       0
          iFunction               0
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        0
          bAlternateSetting       0
          bNumEndpoints           1
          bInterfaceClass        14 Video
          bInterfaceSubClass      1 Video Control
          bInterfaceProtocol      0
          iInterface              0
          VideoControl Interface Descriptor:
            bLength                13
            bDescriptorType        36
            bDescriptorSubtype      1 (HEADER)
            bcdUVC               1.00
            wTotalLength       0x0084
            dwClockFrequency      300.000000MHz
            bInCollection           1
            baInterfaceNr( 0)       1
          VideoControl Interface Descriptor:
            bLength                 9
            bDescriptorType        36
            bDescriptorSubtype      3 (OUTPUT_TERMINAL)
            bTerminalID             4
            wTerminalType      0x0101 USB Streaming
            bAssocTerminal          0
            bSourceID               2
            iTerminal               0
          VideoControl Interface Descriptor:
            bLength                27
            bDescriptorType        36
            bDescriptorSubtype      6 (EXTENSION_UNIT)
            bUnitID                 2
            guidExtensionCode         {2c49d16a-32b8-4485-3ea8-643a152362f2}
            bNumControls            6
            bNrInPins               1
            baSourceID( 0)          6
            bControlSize            2
            bmControls( 0)       0x3f
            bmControls( 1)       0x00
            iExtension              0
          VideoControl Interface Descriptor:
            bLength                27
            bDescriptorType        36
            bDescriptorSubtype      6 (EXTENSION_UNIT)
            bUnitID                 6
            guidExtensionCode         {23e49ed0-1178-4f31-ae52-d2fb8a8d3b48}
            bNumControls            5
            bNrInPins               1
            baSourceID( 0)          5
            bControlSize            2
            bmControls( 0)       0xff
            bmControls( 1)       0x6f
            iExtension              0
          VideoControl Interface Descriptor:
            bLength                27
            bDescriptorType        36
            bDescriptorSubtype      6 (EXTENSION_UNIT)
            bUnitID                 5
            guidExtensionCode         {b42153bd-35d6-45ca-b203-4e0149b301bc}
            bNumControls           16
            bNrInPins               1
            baSourceID( 0)          3
            bControlSize            2
            bmControls( 0)       0x7f
            bmControls( 1)       0x7e
            iExtension              0
          VideoControl Interface Descriptor:
            bLength                18
            bDescriptorType        36
            bDescriptorSubtype      2 (INPUT_TERMINAL)
            bTerminalID             1
            wTerminalType      0x0201 Camera Sensor
            bAssocTerminal          0
            iTerminal               0
            wObjectiveFocalLengthMin      0
            wObjectiveFocalLengthMax      0
            wOcularFocalLength            0
            bControlSize                  3
            bmControls           0x00000a00
              Zoom (Absolute)
              PanTilt (Absolute)
          VideoControl Interface Descriptor:
            bLength                11
            bDescriptorType        36
            bDescriptorSubtype      5 (PROCESSING_UNIT)
          Warning: Descriptor too short
            bUnitID                 3
            bSourceID               1
            wMaxMultiplier          0
            bControlSize            2
            bmControls     0x0000157f
              Brightness
              Contrast
              Hue
              Saturation
              Sharpness
              Gamma
              White Balance Temperature
              Backlight Compensation
              Power Line Frequency
              White Balance Temperature, Auto
            iProcessing             0
            bmVideoStandards     0x4f
              None
              NTSC - 525/60
              PAL - 625/50
              SECAM - 625/50
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x83  EP 3 IN
            bmAttributes            3
              Transfer Type            Interrupt
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0040  1x 64 bytes
            bInterval               8
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        1
          bAlternateSetting       0
          bNumEndpoints           1
          bInterfaceClass        14 Video
          bInterfaceSubClass      2 Video Streaming
          bInterfaceProtocol      0
          iInterface              0
          VideoStreaming Interface Descriptor:
            bLength                            16
            bDescriptorType                    36
            bDescriptorSubtype                  1 (INPUT_HEADER)
            bNumFormats                         3
            wTotalLength                   0x02f4
            bEndpointAddress                 0x81  EP 1 IN
            bmInfo                              0
            bTerminalLink                       4
            bStillCaptureMethod                 0
            bTriggerSupport                     1
            bTriggerUsage                       0
            bControlSize                        1
            bmaControls( 0)                     0
            bmaControls( 1)                     0
            bmaControls( 2)                     0
          VideoStreaming Interface Descriptor:
            bLength                            27
            bDescriptorType                    36
            bDescriptorSubtype                  4 (FORMAT_UNCOMPRESSED)
            bFormatIndex                        1
            bNumFrameDescriptors                7
            guidFormat                            {32595559-0000-0010-8000-00aa00389b71}
            bBitsPerPixel                      16
            bDefaultFrameIndex                  1
            bAspectRatioX                       0
            bAspectRatioY                       0
            bmInterlaceFlags                 0x00
              Interlaced stream or variable: No
              Fields per frame: 2 fields
              Field 1 first: No
              Field pattern: Field 1 only
            bCopyProtect                        0
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         1
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            640
            wHeight                           480
            dwMinBitRate                147456000
            dwMaxBitRate                147456000
            dwMaxVideoFrameBufferSize      614400
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         2
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           2560
            wHeight                          1920
            dwMinBitRate                2359296000
            dwMaxBitRate                2359296000
            dwMaxVideoFrameBufferSize     9830400
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         3
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            320
            wHeight                           240
            dwMinBitRate                 36864000
            dwMaxBitRate                 36864000
            dwMaxVideoFrameBufferSize      153600
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         4
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           1280
            wHeight                           720
            dwMinBitRate                442368000
            dwMaxBitRate                442368000
            dwMaxVideoFrameBufferSize     1843200
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         5
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           1920
            wHeight                          1080
            dwMinBitRate                995328000
            dwMaxBitRate                995328000
            dwMaxVideoFrameBufferSize     4147200
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         6
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            800
            wHeight                           600
            dwMinBitRate                230400000
            dwMaxBitRate                230400000
            dwMaxVideoFrameBufferSize      960000
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         7
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           1440
            wHeight                          1080
            dwMinBitRate                746496000
            dwMaxBitRate                746496000
            dwMaxVideoFrameBufferSize     3110400
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                             6
            bDescriptorType                    36
            bDescriptorSubtype                 13 (COLORFORMAT)
            bColorPrimaries                     1 (BT.709,sRGB)
            bTransferCharacteristics            1 (BT.709)
            bMatrixCoefficients                 4 (SMPTE 170M (BT.601))
          VideoStreaming Interface Descriptor:
            bLength                            11
            bDescriptorType                    36
            bDescriptorSubtype                  6 (FORMAT_MJPEG)
            bFormatIndex                        2
            bNumFrameDescriptors                6
            bFlags                              0
              Fixed-size samples: No
            bDefaultFrameIndex                  1
            bAspectRatioX                       0
            bAspectRatioY                       0
            bmInterlaceFlags                 0x00
              Interlaced stream or variable: No
              Fields per frame: 1 fields
              Field 1 first: No
              Field pattern: Field 1 only
            bCopyProtect                        0
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         1
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            640
            wHeight                           480
            dwMinBitRate                  3686400
            dwMaxBitRate                 94371840
            dwMaxVideoFrameBufferSize     1048576
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         2
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            160
            wHeight                           120
            dwMinBitRate                  3686400
            dwMaxBitRate                 94371840
            dwMaxVideoFrameBufferSize     1048576
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         3
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            320
            wHeight                           240
            dwMinBitRate                  3686400
            dwMaxBitRate                 94371840
            dwMaxVideoFrameBufferSize     1048576
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         4
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           1280
            wHeight                           720
            dwMinBitRate                  3686400
            dwMaxBitRate                 94371840
            dwMaxVideoFrameBufferSize     1048576
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         5
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           2560
            wHeight                          1920
            dwMinBitRate                  1843200
            dwMaxBitRate                 47185920
            dwMaxVideoFrameBufferSize     6291456
            dwDefaultFrameInterval         666666
            bFrameIntervalType                  1
            dwFrameInterval( 0)            666666
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         6
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            800
            wHeight                           600
            dwMinBitRate                  3686400
            dwMaxBitRate                 94371840
            dwMaxVideoFrameBufferSize     1048576
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                             6
            bDescriptorType                    36
            bDescriptorSubtype                 13 (COLORFORMAT)
            bColorPrimaries                     1 (BT.709,sRGB)
            bTransferCharacteristics            1 (BT.709)
            bMatrixCoefficients                 4 (SMPTE 170M (BT.601))
          VideoStreaming Interface Descriptor:
            bLength                            28
            bDescriptorType                    36
            bDescriptorSubtype                 16 (FORMAT_FRAME_BASED)
            bFormatIndex                        3
            bNumFrameDescriptors                7
            guidFormat                            {34363248-0000-0010-8000-00aa00389b71}
            bBitsPerPixel                      16
            bDefaultFrameIndex                  1
            bAspectRatioX                       0
            bAspectRatioY                       0
            bmInterlaceFlags                 0x00
              Interlaced stream or variable: No
              Fields per frame: 2 fields
              Field 1 first: No
              Field pattern: Field 1 only
            bCopyProtect                        0
            bVariableSize                     1
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                 17 (FRAME_FRAME_BASED)
            bFrameIndex                         1
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            640
            wHeight                           480
            dwMinBitRate                125829120
            dwMaxBitRate                251658240
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  3
            dwBytesPerLine                      0
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            416666
            dwFrameInterval( 2)            500000
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                 17 (FRAME_FRAME_BASED)
            bFrameIndex                         2
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            160
            wHeight                           120
            dwMinBitRate                125829120
            dwMaxBitRate                251658240
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  3
            dwBytesPerLine                      0
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            416666
            dwFrameInterval( 2)            500000
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                 17 (FRAME_FRAME_BASED)
            bFrameIndex                         3
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            320
            wHeight                           240
            dwMinBitRate                125829120
            dwMaxBitRate                251658240
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  3
            dwBytesPerLine                      0
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            416666
            dwFrameInterval( 2)            500000
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                 17 (FRAME_FRAME_BASED)
            bFrameIndex                         4
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           1280
            wHeight                           720
            dwMinBitRate                125829120
            dwMaxBitRate                251658240
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  3
            dwBytesPerLine                      0
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            416666
            dwFrameInterval( 2)            500000
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                 17 (FRAME_FRAME_BASED)
            bFrameIndex                         5
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           1920
            wHeight                          1080
            dwMinBitRate                125829120
            dwMaxBitRate                251658240
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  3
            dwBytesPerLine                      0
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            416666
            dwFrameInterval( 2)            500000
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                 17 (FRAME_FRAME_BASED)
            bFrameIndex                         6
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           2560
            wHeight                          1920
            dwMinBitRate                125829120
            dwMaxBitRate                251658240
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  3
            dwBytesPerLine                      0
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            416666
            dwFrameInterval( 2)            500000
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                 17 (FRAME_FRAME_BASED)
            bFrameIndex                         7
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           1440
            wHeight                          1080
            dwMinBitRate                125829120
            dwMaxBitRate                251658240
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  3
            dwBytesPerLine                      0
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            416666
            dwFrameInterval( 2)            500000
          VideoStreaming Interface Descriptor:
            bLength                             6
            bDescriptorType                    36
            bDescriptorSubtype                 13 (COLORFORMAT)
            bColorPrimaries                     1 (BT.709,sRGB)
            bTransferCharacteristics            1 (BT.709)
            bMatrixCoefficients                 4 (SMPTE 170M (BT.601))
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x81  EP 1 IN
            bmAttributes            2
              Transfer Type            Bulk
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0200  1x 512 bytes
            bInterval               0
        Interface Association:
          bLength                 8
          bDescriptorType        11
          bFirstInterface         2
          bInterfaceCount         2
          bFunctionClass          1 Audio
          bFunctionSubClass       2 Streaming
          bFunctionProtocol       0
          iFunction               0
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        2
          bAlternateSetting       0
          bNumEndpoints           0
          bInterfaceClass         1 Audio
          bInterfaceSubClass      1 Control Device
          bInterfaceProtocol      0
          iInterface              0
          AudioControl Interface Descriptor:
            bLength                 9
            bDescriptorType        36
            bDescriptorSubtype      1 (HEADER)
            bcdADC               1.00
            wTotalLength       0x0026
            bInCollection           1
            baInterfaceNr(0)        3
          AudioControl Interface Descriptor:
            bLength                12
            bDescriptorType        36
            bDescriptorSubtype      2 (INPUT_TERMINAL)
            bTerminalID             1
            wTerminalType      0x0201 Microphone
            bAssocTerminal          0
            bNrChannels             2
            wChannelConfig     0x0003
              Left Front (L)
              Right Front (R)
            iChannelNames           0
            iTerminal               0
          AudioControl Interface Descriptor:
            bLength                 9
            bDescriptorType        36
            bDescriptorSubtype      3 (OUTPUT_TERMINAL)
            bTerminalID             3
            wTerminalType      0x0101 USB Streaming
            bAssocTerminal          0
            bSourceID               5
            iTerminal               0
          AudioControl Interface Descriptor:
            bLength                 8
            bDescriptorType        36
            bDescriptorSubtype      6 (FEATURE_UNIT)
            bUnitID                 5
            bSourceID               1
            bControlSize            1
            bmaControls(0)       0x03
              Mute Control
              Volume Control
            iFeature                0
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        3
          bAlternateSetting       0
          bNumEndpoints           0
          bInterfaceClass         1 Audio
          bInterfaceSubClass      2 Streaming
          bInterfaceProtocol      0
          iInterface              0
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        3
          bAlternateSetting       1
          bNumEndpoints           1
          bInterfaceClass         1 Audio
          bInterfaceSubClass      2 Streaming
          bInterfaceProtocol      0
          iInterface              0
          AudioStreaming Interface Descriptor:
            bLength                 7
            bDescriptorType        36
            bDescriptorSubtype      1 (AS_GENERAL)
            bTerminalLink           3
            bDelay                  2 frames
            wFormatTag         0x0001 PCM
          AudioStreaming Interface Descriptor:
            bLength                17
            bDescriptorType        36
            bDescriptorSubtype      2 (FORMAT_TYPE)
            bFormatType             1 (FORMAT_TYPE_I)
            bNrChannels             2
            bSubframeSize           2
            bBitResolution         16
            bSamFreqType            3 Discrete
            tSamFreq[ 0]        16000
            tSamFreq[ 1]        24000
            tSamFreq[ 2]        48000
          Endpoint Descriptor:
            bLength                 9
            bDescriptorType         5
            bEndpointAddress     0x82  EP 2 IN
            bmAttributes            5
              Transfer Type            Isochronous
              Synch Type               Asynchronous
              Usage Type               Data
            wMaxPacketSize     0x0200  1x 512 bytes
            bInterval               4
            bRefresh                0
            bSynchAddress           0
            AudioStreaming Endpoint Descriptor:
              bLength                 7
              bDescriptorType        37
              bDescriptorSubtype      1 (EP_GENERAL)
              bmAttributes         0x01
                Sampling Frequency
              bLockDelayUnits         0 Undefined
              wLockDelay         0x0000
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        4
          bAlternateSetting       0
          bNumEndpoints           2
          bInterfaceClass       255 Vendor Specific Class
          bInterfaceSubClass    255 Vendor Specific Subclass
          bInterfaceProtocol      0
          iInterface              0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x84  EP 4 IN
            bmAttributes            2
              Transfer Type            Bulk
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0200  1x 512 bytes
            bInterval               0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x04  EP 4 OUT
            bmAttributes            2
              Transfer Type            Bulk
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0200  1x 512 bytes
            bInterval               0
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        5
          bAlternateSetting       0
          bNumEndpoints           2
          bInterfaceClass       255 Vendor Specific Class
          bInterfaceSubClass    255 Vendor Specific Subclass
          bInterfaceProtocol      0
          iInterface              0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x85  EP 5 IN
            bmAttributes            2
              Transfer Type            Bulk
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0200  1x 512 bytes
            bInterval               0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x05  EP 5 OUT
            bmAttributes            2
              Transfer Type            Bulk
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0200  1x 512 bytes
            bInterval               0
    Device Qualifier (for other device speed):
      bLength                10
      bDescriptorType         6
      bcdUSB               2.00
      bDeviceClass          239 Miscellaneous Device
      bDeviceSubClass         2
      bDeviceProtocol         1 Interface Association
      bMaxPacketSize0        64
      bNumConfigurations      1
    Device Status:     0x0000
      (Bus Powered)

### VG-STC5000
 * 1920x1080/30fps
 * Alpha Imaging Tech. AIT5859B (firmware begins with "AIT842SF" string)

lsusb (no firmware loaded):

    Bus 001 Device 002: ID 04e8:2065 Samsung Electronics Co., Ltd USB2.0 UVC HQ WebCam
    Device Descriptor:
      bLength                18
      bDescriptorType         1
      bcdUSB               2.00
      bDeviceClass          239 Miscellaneous Device
      bDeviceSubClass         2
      bDeviceProtocol         1 Interface Association
      bMaxPacketSize0        64
      idVendor           0x04e8 Samsung Electronics Co., Ltd
      idProduct          0x2065
      bcdDevice            9.02
      iManufacturer           1 Alpha Imaging Tech. Corp.
      iProduct                2 USB2.0 UVC HQ WebCam
      iSerial                 0
      bNumConfigurations      1
      Configuration Descriptor:
        bLength                 9
        bDescriptorType         2
        wTotalLength       0x0193
        bNumInterfaces          4
        bConfigurationValue     1
        iConfiguration          0
        bmAttributes         0x80
          (Bus Powered)
        MaxPower              500mA
        Interface Association:
          bLength                 8
          bDescriptorType        11
          bFirstInterface         0
          bInterfaceCount         2
          bFunctionClass         14 Video
          bFunctionSubClass       3 Video Interface Collection
          bFunctionProtocol       0
          iFunction               0
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        0
          bAlternateSetting       0
          bNumEndpoints           1
          bInterfaceClass        14 Video
          bInterfaceSubClass      1 Video Control
          bInterfaceProtocol      0
          iInterface              0
          VideoControl Interface Descriptor:
            bLength                13
            bDescriptorType        36
            bDescriptorSubtype      1 (HEADER)
            bcdUVC               1.00
            wTotalLength       0x0069
            dwClockFrequency       30.000000MHz
            bInCollection           1
            baInterfaceNr( 0)       1
          VideoControl Interface Descriptor:
            bLength                 9
            bDescriptorType        36
            bDescriptorSubtype      3 (OUTPUT_TERMINAL)
            bTerminalID             4
            wTerminalType      0x0101 USB Streaming
            bAssocTerminal          0
            bSourceID               2
            iTerminal               0
          VideoControl Interface Descriptor:
            bLength                27
            bDescriptorType        36
            bDescriptorSubtype      6 (EXTENSION_UNIT)
            bUnitID                 2
            guidExtensionCode         {2c49d16a-32b8-4485-3ea8-643a152362f2}
            bNumControls            6
            bNrInPins               1
            baSourceID( 0)          6
            bControlSize            2
            bmControls( 0)       0x3f
            bmControls( 1)       0x00
            iExtension              0
          VideoControl Interface Descriptor:
            bLength                27
            bDescriptorType        36
            bDescriptorSubtype      6 (EXTENSION_UNIT)
            bUnitID                 6
            guidExtensionCode         {23e49ed0-1178-4f31-ae52-d2fb8a8d3b48}
            bNumControls            5
            bNrInPins               1
            baSourceID( 0)          3
            bControlSize            2
            bmControls( 0)       0xff
            bmControls( 1)       0x6f
            iExtension              0
          VideoControl Interface Descriptor:
            bLength                18
            bDescriptorType        36
            bDescriptorSubtype      2 (INPUT_TERMINAL)
            bTerminalID             1
            wTerminalType      0x0201 Camera Sensor
            bAssocTerminal          0
            iTerminal               0
            wObjectiveFocalLengthMin      0
            wObjectiveFocalLengthMax      0
            wOcularFocalLength            0
            bControlSize                  3
            bmControls           0x00000a0e
              Auto-Exposure Mode
              Auto-Exposure Priority
              Exposure Time (Absolute)
              Zoom (Absolute)
              PanTilt (Absolute)
          VideoControl Interface Descriptor:
            bLength                11
            bDescriptorType        36
            bDescriptorSubtype      5 (PROCESSING_UNIT)
          Warning: Descriptor too short
            bUnitID                 3
            bSourceID               1
            wMaxMultiplier          0
            bControlSize            2
            bmControls     0x0000177f
              Brightness
              Contrast
              Hue
              Saturation
              Sharpness
              Gamma
              White Balance Temperature
              Backlight Compensation
              Gain
              Power Line Frequency
              White Balance Temperature, Auto
            iProcessing             0
            bmVideoStandards     0x00
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x83  EP 3 IN
            bmAttributes            3
              Transfer Type            Interrupt
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0008  1x 8 bytes
            bInterval               8
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        1
          bAlternateSetting       0
          bNumEndpoints           1
          bInterfaceClass        14 Video
          bInterfaceSubClass      2 Video Streaming
          bInterfaceProtocol      0
          iInterface              0
          VideoStreaming Interface Descriptor:
            bLength                            16
            bDescriptorType                    36
            bDescriptorSubtype                  1 (INPUT_HEADER)
            bNumFormats                         3
            wTotalLength                   0x00c6
            bEndpointAddress                 0x84  EP 4 IN
            bmInfo                              0
            bTerminalLink                       4
            bStillCaptureMethod                 0
            bTriggerSupport                     1
            bTriggerUsage                       0
            bControlSize                        1
            bmaControls( 0)                     0
            bmaControls( 1)                     0
            bmaControls( 2)                     0
          VideoStreaming Interface Descriptor:
            bLength                            27
            bDescriptorType                    36
            bDescriptorSubtype                  4 (FORMAT_UNCOMPRESSED)
            bFormatIndex                        1
            bNumFrameDescriptors                1
            guidFormat                            {32595559-0000-0010-8000-00aa00389b71}
            bBitsPerPixel                      16
            bDefaultFrameIndex                  1
            bAspectRatioX                       0
            bAspectRatioY                       0
            bmInterlaceFlags                 0x00
              Interlaced stream or variable: No
              Fields per frame: 2 fields
              Field 1 first: No
              Field pattern: Field 1 only
            bCopyProtect                        0
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         1
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            640
            wHeight                           480
            dwMinBitRate                147456000
            dwMaxBitRate                147456000
            dwMaxVideoFrameBufferSize      614400
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                             6
            bDescriptorType                    36
            bDescriptorSubtype                 13 (COLORFORMAT)
            bColorPrimaries                     1 (BT.709,sRGB)
            bTransferCharacteristics            1 (BT.709)
            bMatrixCoefficients                 4 (SMPTE 170M (BT.601))
          VideoStreaming Interface Descriptor:
            bLength                            11
            bDescriptorType                    36
            bDescriptorSubtype                  6 (FORMAT_MJPEG)
            bFormatIndex                        2
            bNumFrameDescriptors                1
            bFlags                              0
              Fixed-size samples: No
            bDefaultFrameIndex                  1
            bAspectRatioX                       0
            bAspectRatioY                       0
            bmInterlaceFlags                 0x00
              Interlaced stream or variable: No
              Fields per frame: 1 fields
              Field 1 first: No
              Field pattern: Field 1 only
            bCopyProtect                        0
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         1
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            640
            wHeight                           480
            dwMinBitRate                  3686400
            dwMaxBitRate                 94371840
            dwMaxVideoFrameBufferSize     1048576
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                             6
            bDescriptorType                    36
            bDescriptorSubtype                 13 (COLORFORMAT)
            bColorPrimaries                     1 (BT.709,sRGB)
            bTransferCharacteristics            1 (BT.709)
            bMatrixCoefficients                 4 (SMPTE 170M (BT.601))
          VideoStreaming Interface Descriptor:
            bLength                            28
            bDescriptorType                    36
            bDescriptorSubtype                 16 (FORMAT_FRAME_BASED)
            bFormatIndex                        3
            bNumFrameDescriptors                1
            guidFormat                            {34363248-0000-0010-8000-00aa00389b71}
            bBitsPerPixel                      16
            bDefaultFrameIndex                  1
            bAspectRatioX                       0
            bAspectRatioY                       0
            bmInterlaceFlags                 0x00
              Interlaced stream or variable: No
              Fields per frame: 2 fields
              Field 1 first: No
              Field pattern: Field 1 only
            bCopyProtect                        0
            bVariableSize                     1
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                 17 (FRAME_FRAME_BASED)
            bFrameIndex                         1
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            640
            wHeight                           480
            dwMinBitRate                125829120
            dwMaxBitRate                251658240
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  3
            dwBytesPerLine                      0
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            416666
            dwFrameInterval( 2)            500000
          VideoStreaming Interface Descriptor:
            bLength                             6
            bDescriptorType                    36
            bDescriptorSubtype                 13 (COLORFORMAT)
            bColorPrimaries                     1 (BT.709,sRGB)
            bTransferCharacteristics            1 (BT.709)
            bMatrixCoefficients                 4 (SMPTE 170M (BT.601))
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x84  EP 4 IN
            bmAttributes            2
              Transfer Type            Bulk
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0200  1x 512 bytes
            bInterval               0
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        2
          bAlternateSetting       0
          bNumEndpoints           2
          bInterfaceClass       255 Vendor Specific Class
          bInterfaceSubClass    255 Vendor Specific Subclass
          bInterfaceProtocol      0
          iInterface              0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x81  EP 1 IN
            bmAttributes            2
              Transfer Type            Bulk
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0200  1x 512 bytes
            bInterval               0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x01  EP 1 OUT
            bmAttributes            2
              Transfer Type            Bulk
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0200  1x 512 bytes
            bInterval               0
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        3
          bAlternateSetting       0
          bNumEndpoints           2
          bInterfaceClass       255 Vendor Specific Class
          bInterfaceSubClass    255 Vendor Specific Subclass
          bInterfaceProtocol      0
          iInterface              0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x85  EP 5 IN
            bmAttributes            2
              Transfer Type            Bulk
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0200  1x 512 bytes
            bInterval               0
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x05  EP 5 OUT
            bmAttributes            2
              Transfer Type            Bulk
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0200  1x 512 bytes
            bInterval               0
    Device Qualifier (for other device speed):
      bLength                10
      bDescriptorType         6
      bcdUSB               2.00
      bDeviceClass          239 Miscellaneous Device
      bDeviceSubClass         2
      bDeviceProtocol         1 Interface Association
      bMaxPacketSize0        64
      bNumConfigurations      1
    Device Status:     0x0000
      (Bus Powered)

lsusb (firmware loaded):

    Bus 001 Device 003: ID 04e8:2066 Samsung Electronics Co., Ltd SAMSUNG VD USB HDCAM
    Device Descriptor:
      bLength                18
      bDescriptorType         1
      bcdUSB               2.00
      bDeviceClass          239 Miscellaneous Device
      bDeviceSubClass         2
      bDeviceProtocol         1 Interface Association
      bMaxPacketSize0        64
      idVendor           0x04e8 Samsung Electronics Co., Ltd
      idProduct          0x2066
      bcdDevice            7.26
      iManufacturer           1 Alpha Imaging Tech. Corp.
      iProduct                2 SAMSUNG VD USB HDCAM
      iSerial                 0
      bNumConfigurations      1
      Configuration Descriptor:
        bLength                 9
        bDescriptorType         2
        wTotalLength       0x04ee
        bNumInterfaces          4
        bConfigurationValue     1
        iConfiguration          0
        bmAttributes         0x80
          (Bus Powered)
        MaxPower              500mA
        Interface Association:
          bLength                 8
          bDescriptorType        11
          bFirstInterface         0
          bInterfaceCount         2
          bFunctionClass         14 Video
          bFunctionSubClass       3 Video Interface Collection
          bFunctionProtocol       0
          iFunction               0
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        0
          bAlternateSetting       0
          bNumEndpoints           1
          bInterfaceClass        14 Video
          bInterfaceSubClass      1 Video Control
          bInterfaceProtocol      0
          iInterface              0
          VideoControl Interface Descriptor:
            bLength                13
            bDescriptorType        36
            bDescriptorSubtype      1 (HEADER)
            bcdUVC               1.00
            wTotalLength       0x0086
            dwClockFrequency       30.000000MHz
            bInCollection           1
            baInterfaceNr( 0)       1
          VideoControl Interface Descriptor:
            bLength                 9
            bDescriptorType        36
            bDescriptorSubtype      3 (OUTPUT_TERMINAL)
            bTerminalID             4
            wTerminalType      0x0101 USB Streaming
            bAssocTerminal          0
            bSourceID               2
            iTerminal               0
          VideoControl Interface Descriptor:
            bLength                27
            bDescriptorType        36
            bDescriptorSubtype      6 (EXTENSION_UNIT)
            bUnitID                 2
            guidExtensionCode         {2c49d16a-32b8-4485-3ea8-643a152362f2}
            bNumControls            6
            bNrInPins               1
            baSourceID( 0)          6
            bControlSize            2
            bmControls( 0)       0x3f
            bmControls( 1)       0x00
            iExtension              0
          VideoControl Interface Descriptor:
            bLength                27
            bDescriptorType        36
            bDescriptorSubtype      6 (EXTENSION_UNIT)
            bUnitID                 6
            guidExtensionCode         {23e49ed0-1178-4f31-ae52-d2fb8a8d3b48}
            bNumControls            5
            bNrInPins               1
            baSourceID( 0)          7
            bControlSize            2
            bmControls( 0)       0xff
            bmControls( 1)       0x6f
            iExtension              0
          VideoControl Interface Descriptor:
            bLength                29
            bDescriptorType        36
            bDescriptorSubtype      6 (EXTENSION_UNIT)
            bUnitID                 7
            guidExtensionCode         {bd5321b4-d635-ca45-b203-4e0149b301bc}
            bNumControls           32
            bNrInPins               1
            baSourceID( 0)          3
            bControlSize            4
            bmControls( 0)       0xff
            bmControls( 1)       0x3f
            bmControls( 2)       0x80
            bmControls( 3)       0x07
            iExtension              0
          VideoControl Interface Descriptor:
            bLength                18
            bDescriptorType        36
            bDescriptorSubtype      2 (INPUT_TERMINAL)
            bTerminalID             1
            wTerminalType      0x0201 Camera Sensor
            bAssocTerminal          0
            iTerminal               0
            wObjectiveFocalLengthMin      0
            wObjectiveFocalLengthMax      0
            wOcularFocalLength            0
            bControlSize                  3
            bmControls           0x00000a00
              Zoom (Absolute)
              PanTilt (Absolute)
          VideoControl Interface Descriptor:
            bLength                11
            bDescriptorType        36
            bDescriptorSubtype      5 (PROCESSING_UNIT)
          Warning: Descriptor too short
            bUnitID                 3
            bSourceID               1
            wMaxMultiplier          0
            bControlSize            2
            bmControls     0x0000157f
              Brightness
              Contrast
              Hue
              Saturation
              Sharpness
              Gamma
              White Balance Temperature
              Backlight Compensation
              Power Line Frequency
              White Balance Temperature, Auto
            iProcessing             0
            bmVideoStandards     0x6e
              NTSC - 525/60
              PAL - 625/50
              SECAM - 625/50
              PAL - 525/60
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x83  EP 3 IN
            bmAttributes            3
              Transfer Type            Interrupt
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0040  1x 64 bytes
            bInterval               8
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        1
          bAlternateSetting       0
          bNumEndpoints           1
          bInterfaceClass        14 Video
          bInterfaceSubClass      2 Video Streaming
          bInterfaceProtocol      0
          iInterface              0
          VideoStreaming Interface Descriptor:
            bLength                            17
            bDescriptorType                    36
            bDescriptorSubtype                  1 (INPUT_HEADER)
            bNumFormats                         4
            wTotalLength                   0x03c4
            bEndpointAddress                 0x81  EP 1 IN
            bmInfo                              0
            bTerminalLink                       4
            bStillCaptureMethod                 0
            bTriggerSupport                     1
            bTriggerUsage                       0
            bControlSize                        1
            bmaControls( 0)                     0
            bmaControls( 1)                     0
            bmaControls( 2)                     0
            bmaControls( 3)                     0
          VideoStreaming Interface Descriptor:
            bLength                            27
            bDescriptorType                    36
            bDescriptorSubtype                  4 (FORMAT_UNCOMPRESSED)
            bFormatIndex                        1
            bNumFrameDescriptors                8
            guidFormat                            {32595559-0000-0010-8000-00aa00389b71}
            bBitsPerPixel                      16
            bDefaultFrameIndex                  1
            bAspectRatioX                       0
            bAspectRatioY                       0
            bmInterlaceFlags                 0x00
              Interlaced stream or variable: No
              Fields per frame: 2 fields
              Field 1 first: No
              Field pattern: Field 1 only
            bCopyProtect                        0
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         1
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            640
            wHeight                           480
            dwMinBitRate                147456000
            dwMaxBitRate                147456000
            dwMaxVideoFrameBufferSize      614400
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         2
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           2560
            wHeight                          1920
            dwMinBitRate                2359296000
            dwMaxBitRate                2359296000
            dwMaxVideoFrameBufferSize     9830400
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         3
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            320
            wHeight                           240
            dwMinBitRate                 36864000
            dwMaxBitRate                 36864000
            dwMaxVideoFrameBufferSize      153600
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         4
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           1280
            wHeight                           720
            dwMinBitRate                442368000
            dwMaxBitRate                442368000
            dwMaxVideoFrameBufferSize     1843200
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         5
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           1920
            wHeight                          1080
            dwMinBitRate                995328000
            dwMaxBitRate                995328000
            dwMaxVideoFrameBufferSize     4147200
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         6
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            800
            wHeight                           600
            dwMinBitRate                230400000
            dwMaxBitRate                230400000
            dwMaxVideoFrameBufferSize      960000
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         7
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           1440
            wHeight                          1080
            dwMinBitRate                746496000
            dwMaxBitRate                746496000
            dwMaxVideoFrameBufferSize     3110400
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         8
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            864
            wHeight                           486
            dwMinBitRate                201553920
            dwMaxBitRate                201553920
            dwMaxVideoFrameBufferSize      839808
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                             6
            bDescriptorType                    36
            bDescriptorSubtype                 13 (COLORFORMAT)
            bColorPrimaries                     1 (BT.709,sRGB)
            bTransferCharacteristics            1 (BT.709)
            bMatrixCoefficients                 4 (SMPTE 170M (BT.601))
          VideoStreaming Interface Descriptor:
            bLength                            11
            bDescriptorType                    36
            bDescriptorSubtype                  6 (FORMAT_MJPEG)
            bFormatIndex                        3
            bNumFrameDescriptors                6
            bFlags                              0
              Fixed-size samples: No
            bDefaultFrameIndex                  1
            bAspectRatioX                       0
            bAspectRatioY                       0
            bmInterlaceFlags                 0x00
              Interlaced stream or variable: No
              Fields per frame: 1 fields
              Field 1 first: No
              Field pattern: Field 1 only
            bCopyProtect                        0
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         1
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            640
            wHeight                           480
            dwMinBitRate                  3686400
            dwMaxBitRate                 94371840
            dwMaxVideoFrameBufferSize     1048576
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         2
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            160
            wHeight                           120
            dwMinBitRate                  3686400
            dwMaxBitRate                 94371840
            dwMaxVideoFrameBufferSize     1048576
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         3
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            320
            wHeight                           240
            dwMinBitRate                  3686400
            dwMaxBitRate                 94371840
            dwMaxVideoFrameBufferSize     1048576
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         4
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           1280
            wHeight                           720
            dwMinBitRate                  3686400
            dwMaxBitRate                 94371840
            dwMaxVideoFrameBufferSize     1048576
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         6
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           2560
            wHeight                          1920
            dwMinBitRate                  1843200
            dwMaxBitRate                 47185920
            dwMaxVideoFrameBufferSize     6291456
            dwDefaultFrameInterval         666666
            bFrameIntervalType                  1
            dwFrameInterval( 0)            666666
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         5
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           1920
            wHeight                          1080
            dwMinBitRate                  3686400
            dwMaxBitRate                 94371840
            dwMaxVideoFrameBufferSize     1376256
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                             6
            bDescriptorType                    36
            bDescriptorSubtype                 13 (COLORFORMAT)
            bColorPrimaries                     1 (BT.709,sRGB)
            bTransferCharacteristics            1 (BT.709)
            bMatrixCoefficients                 4 (SMPTE 170M (BT.601))
          VideoStreaming Interface Descriptor:
            bLength                            28
            bDescriptorType                    36
            bDescriptorSubtype                 16 (FORMAT_FRAME_BASED)
            bFormatIndex                        4
            bNumFrameDescriptors               10
            guidFormat                            {34363248-0000-0010-8000-00aa00389b71}
            bBitsPerPixel                      16
            bDefaultFrameIndex                  1
            bAspectRatioX                       0
            bAspectRatioY                       0
            bmInterlaceFlags                 0x00
              Interlaced stream or variable: No
              Fields per frame: 2 fields
              Field 1 first: No
              Field pattern: Field 1 only
            bCopyProtect                        0
            bVariableSize                     1
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                 17 (FRAME_FRAME_BASED)
            bFrameIndex                         1
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            640
            wHeight                           480
            dwMinBitRate                125829120
            dwMaxBitRate                251658240
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  3
            dwBytesPerLine                      0
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            416666
            dwFrameInterval( 2)            500000
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                 17 (FRAME_FRAME_BASED)
            bFrameIndex                         2
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            160
            wHeight                           120
            dwMinBitRate                125829120
            dwMaxBitRate                251658240
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  3
            dwBytesPerLine                      0
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            416666
            dwFrameInterval( 2)            500000
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                 17 (FRAME_FRAME_BASED)
            bFrameIndex                         3
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            320
            wHeight                           240
            dwMinBitRate                125829120
            dwMaxBitRate                251658240
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  3
            dwBytesPerLine                      0
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            416666
            dwFrameInterval( 2)            500000
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                 17 (FRAME_FRAME_BASED)
            bFrameIndex                         4
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           1280
            wHeight                           720
            dwMinBitRate                125829120
            dwMaxBitRate                251658240
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  3
            dwBytesPerLine                      0
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            416666
            dwFrameInterval( 2)            500000
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                 17 (FRAME_FRAME_BASED)
            bFrameIndex                         5
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           1920
            wHeight                          1080
            dwMinBitRate                125829120
            dwMaxBitRate                251658240
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  3
            dwBytesPerLine                      0
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            416666
            dwFrameInterval( 2)            500000
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                 17 (FRAME_FRAME_BASED)
            bFrameIndex                         6
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           2560
            wHeight                          1920
            dwMinBitRate                125829120
            dwMaxBitRate                251658240
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  3
            dwBytesPerLine                      0
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            416666
            dwFrameInterval( 2)            500000
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                 17 (FRAME_FRAME_BASED)
            bFrameIndex                         7
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                           1440
            wHeight                          1080
            dwMinBitRate                125829120
            dwMaxBitRate                251658240
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  3
            dwBytesPerLine                      0
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            416666
            dwFrameInterval( 2)            500000
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                 17 (FRAME_FRAME_BASED)
            bFrameIndex                         8
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            160
            wHeight                            90
            dwMinBitRate                125829120
            dwMaxBitRate                251658240
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  3
            dwBytesPerLine                      0
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            416666
            dwFrameInterval( 2)            500000
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                 17 (FRAME_FRAME_BASED)
            bFrameIndex                         9
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            640
            wHeight                           360
            dwMinBitRate                125829120
            dwMaxBitRate                251658240
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  3
            dwBytesPerLine                      0
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            416666
            dwFrameInterval( 2)            500000
          VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                 17 (FRAME_FRAME_BASED)
            bFrameIndex                        10
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            320
            wHeight                           180
            dwMinBitRate                125829120
            dwMaxBitRate                251658240
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  3
            dwBytesPerLine                      0
            dwFrameInterval( 0)            333333
            dwFrameInterval( 1)            416666
            dwFrameInterval( 2)            500000
          VideoStreaming Interface Descriptor:
            bLength                             6
            bDescriptorType                    36
            bDescriptorSubtype                 13 (COLORFORMAT)
            bColorPrimaries                     1 (BT.709,sRGB)
            bTransferCharacteristics            1 (BT.709)
            bMatrixCoefficients                 4 (SMPTE 170M (BT.601))
          VideoStreaming Interface Descriptor:
            bLength                            27
            bDescriptorType                    36
            bDescriptorSubtype                  4 (FORMAT_UNCOMPRESSED)
            bFormatIndex                        2
            bNumFrameDescriptors                1
            guidFormat                            {3231564e-0000-0010-8000-00aa00389b71}
            bBitsPerPixel                      12
            bDefaultFrameIndex                  1
            bAspectRatioX                       0
            bAspectRatioY                       0
            bmInterlaceFlags                 0x00
              Interlaced stream or variable: No
              Fields per frame: 2 fields
              Field 1 first: No
              Field pattern: Field 1 only
            bCopyProtect                        0
          VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         1
            bmCapabilities                   0x00
              Still image unsupported
            wWidth                            864
            wHeight                           486
            dwMinBitRate                151165440
            dwMaxBitRate                151165440
            dwMaxVideoFrameBufferSize      629856
            dwDefaultFrameInterval         333333
            bFrameIntervalType                  1
            dwFrameInterval( 0)            333333
          VideoStreaming Interface Descriptor:
            bLength                             6
            bDescriptorType                    36
            bDescriptorSubtype                 13 (COLORFORMAT)
            bColorPrimaries                     1 (BT.709,sRGB)
            bTransferCharacteristics            1 (BT.709)
            bMatrixCoefficients                 4 (SMPTE 170M (BT.601))
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x81  EP 1 IN
            bmAttributes            2
              Transfer Type            Bulk
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x0200  1x 512 bytes
            bInterval               0
        Interface Association:
          bLength                 8
          bDescriptorType        11
          bFirstInterface         2
          bInterfaceCount         2
          bFunctionClass          1 Audio
          bFunctionSubClass       2 Streaming
          bFunctionProtocol       0
          iFunction               0
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        2
          bAlternateSetting       0
          bNumEndpoints           0
          bInterfaceClass         1 Audio
          bInterfaceSubClass      1 Control Device
          bInterfaceProtocol      0
          iInterface              0
          AudioControl Interface Descriptor:
            bLength                 9
            bDescriptorType        36
            bDescriptorSubtype      1 (HEADER)
            bcdADC               1.00
            wTotalLength       0x0026
            bInCollection           1
            baInterfaceNr(0)        3
          AudioControl Interface Descriptor:
            bLength                12
            bDescriptorType        36
            bDescriptorSubtype      2 (INPUT_TERMINAL)
            bTerminalID             1
            wTerminalType      0x0201 Microphone
            bAssocTerminal          0
            bNrChannels             2
            wChannelConfig     0x0003
              Left Front (L)
              Right Front (R)
            iChannelNames           0
            iTerminal               0
          AudioControl Interface Descriptor:
            bLength                 9
            bDescriptorType        36
            bDescriptorSubtype      3 (OUTPUT_TERMINAL)
            bTerminalID             3
            wTerminalType      0x0101 USB Streaming
            bAssocTerminal          0
            bSourceID               5
            iTerminal               0
          AudioControl Interface Descriptor:
            bLength                 8
            bDescriptorType        36
            bDescriptorSubtype      6 (FEATURE_UNIT)
            bUnitID                 5
            bSourceID               1
            bControlSize            1
            bmaControls(0)       0x03
              Mute Control
              Volume Control
            iFeature                0
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        3
          bAlternateSetting       0
          bNumEndpoints           0
          bInterfaceClass         1 Audio
          bInterfaceSubClass      2 Streaming
          bInterfaceProtocol      0
          iInterface              0
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        3
          bAlternateSetting       1
          bNumEndpoints           1
          bInterfaceClass         1 Audio
          bInterfaceSubClass      2 Streaming
          bInterfaceProtocol      0
          iInterface              0
          AudioStreaming Interface Descriptor:
            bLength                 7
            bDescriptorType        36
            bDescriptorSubtype      1 (AS_GENERAL)
            bTerminalLink           3
            bDelay                  2 frames
            wFormatTag         0x0001 PCM
          AudioStreaming Interface Descriptor:
            bLength                14
            bDescriptorType        36
            bDescriptorSubtype      2 (FORMAT_TYPE)
            bFormatType             1 (FORMAT_TYPE_I)
            bNrChannels             2
            bSubframeSize           2
            bBitResolution         16
            bSamFreqType            2 Discrete
            tSamFreq[ 0]        16000
            tSamFreq[ 1]        24000
          Endpoint Descriptor:
            bLength                 9
            bDescriptorType         5
            bEndpointAddress     0x82  EP 2 IN
            bmAttributes            5
              Transfer Type            Isochronous
              Synch Type               Asynchronous
              Usage Type               Data
            wMaxPacketSize     0x0200  1x 512 bytes
            bInterval               4
            bRefresh                0
            bSynchAddress           0
            AudioStreaming Endpoint Descriptor:
              bLength                 7
              bDescriptorType        37
              bDescriptorSubtype      1 (EP_GENERAL)
              bmAttributes         0x01
                Sampling Frequency
              bLockDelayUnits         0 Undefined
              wLockDelay         0x0000
    Device Qualifier (for other device speed):
      bLength                10
      bDescriptorType         6
      bcdUSB               2.00
      bDeviceClass          239 Miscellaneous Device
      bDeviceSubClass         2
      bDeviceProtocol         1 Interface Association
      bMaxPacketSize0        64
      bNumConfigurations      1
    Device Status:     0x0000
      (Bus Powered)
