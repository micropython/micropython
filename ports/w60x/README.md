MicroPython port to the W60X
=============================

W60X is an embedded Wi-Fi SoC chip which is complying with IEEE802.11b/g/n 
international standard and which supports multi interface, multi protocol. 
It can be easily applied to smart appliances, smart home, health care, 
smart toy, wireless audio & video, industrial and other IoT fields. 
This SoC integrates Cortex-M3 CPU, Flash, RF Transceiver, CMOS PA, BaseBand. 
It applies multi interfaces such as SPI, UART, GPIO, I2C, PWM, I2S, 7816. 
It applies multi encryption and decryption protocol 
such as PRNG/SHA1/MD5/RC4/DES/3DES/AES/CRC/RSA.

This is an experimental port of MicroPython to the WinnerMicro W60X microcontroller.  

Supported features
------------------------------------

- REPL (Python prompt) over UART0.
- 16k stack for the MicroPython task and 45k Python heap.
- Many of MicroPython's features are enabled: unicode, arbitrary-precision
  integers, single-precision floats, complex numbers, frozen bytecode, as
  well as many of the internal modules.
- The machine module with GPIO, UART, software SPI, I2C, PWM, WDT and Timer.
- The network module with WLAN (WiFi) support.
- Internal filesystem using the flash (27Kb available).
- Support built-in FTP server transfer script files.

Setting up the cross toolchain and WM_SDK
------------------------------------

Supports direct compilation in Linux system and
compilation in Cygwin environment in Windows system.

There are two main things to do here:
- Download the cross toolchain and add it to the environment variable
- Download WM_SDK and add to environment variables

The cross toolchain used is arm-none-eabi-gcc where the download address is
[GNU Arm Embedded Toolchain](https://launchpad.net/gcc-arm-embedded/4.9/4.9-2014-q4-major)

You will need to update your `PATH`
environment variable to include the cross toolchain. For example, you can issue
the following commands on (at least) Linux:

    $ export PATH=$PATH:/opt/tools/arm-none-eabi-gcc/bin

You can put this command in your `.profile` or `.bash_login`.

WM_SDK download address is [W60X_SDK](http://www.winnermicro.com/en/html/1/).
WM_SDK must be G3.1 and newer versions.

You will need to update your `PATH`
environment variable to include the path of WM_SDK. For example, you can issue
the following commands on (at least) Linux:

    $ export WMSDK_PATH=$PATH:/home/username/WM_SDK

You can put this command in your `.profile` or `.bash_login`.

You also need to modify the build configuration file in WM_SDK, located at:
`WM_SDK/Include/wm_config.h`
You can crop the component by modifying the macro switch, For example, 

    #define TLS_CONFIG_HOSTIF CFG_OFF

The recommended components that can be turned off are:

    #define TLS_CONFIG_HS_SPI      CFG_OFF
    #define TLS_CONFIG_HOSTIF      CFG_OFF
    #define TLS_CONFIG_RMMS        CFG_OFF
    #define TLS_CONFIG_HTTP_CLIENT CFG_OFF
    #define TLS_CONFIG_NTP         CFG_OFF
    
Building the firmware
---------------------

Build MicroPython for the W60X:
```bash
$ cd ports/w60x
$ make V=s
```
This will produce binary firmware images in the `build/` subdirectory
(Recommended use of W600_GZ.img, and the size cannot exceed 352kb).

Flashing the Firmware
-----------------------

This will use the `download.py` script to download the
binary images. For example, 

    python ./download.py ttyUSB0 ./build/W600_GZ.img

Reference document
-----------------------
Visit [WinnerMicro](http://www.winnermicro.com/en/html/1/156/158/497.html) for more documentation.

