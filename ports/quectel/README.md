MicroPython port to the Quectel
=============================

This is a port of MicroPython to the Quectel series of
microcontrollers.  It uses the HeliosSDK and MicroPython runs as
a task under FreeRTOS.

Support Eigencomm, Unisoc, Qualcomm and ASR cellular modules.

Supported features include:
- REPL (Python prompt) over usb.
- Python 3.4 syntax and built-in rich functional modules.
- The machine module with GPIO, EXTINT, UART, SPI, ADC, WDT RTC, and Timer.
- The network module with cellular modem support.
- etc.

Setting up HeliosSDK and the build environment
--------------------------------------------

MicroPython on quectel port requires the HeliosSDK.  The HeliosSDK includes the libraries and RTOS needed to
manage the quectel microcontroller, as well as a way to manage the required
build environment and toolchains needed to build the firmware.

To install the HeliosSDK the full instructions can be found at the
[HeliosSDK Development Guide](https://python.quectel.com/doc/Application_guide/zh/helios-sdk/quick-start.html).

**Windows 10 Environment Setup**

*Step 1: Download Toolchain*

Download the toolchain installation package [helios-toolchain.exe](https://github.com/QuecPython/toolchain/releases/tag/V1.4.2) for Windows 10 environment from the QuecPython official website.

*Step 2: Install Toolchain*

Run helios-toolchain.exe as an administrator, as shown in the following figure, and click "**Install**" to install the toolchain.

> The target folder **must not** contain spaces.

**Ubuntu Environment Setup**

*Step 1: Download Toolchain*

Download the toolchain installation package [helios-toolchain](https://github.com/QuecPython/toolchain/releases/tag/V1.1.0) for Ubuntu 16.04 environment from the QuecPython official website.

*Step 2: Install Toolchain*

Place the installation package in the same location as the target folder. Execute the following command to install the toolchain.
`sudo ./helios-toolchain`

*Step 3: Install Other Tools*

Enter the following command in the terminal to install `p7zip-full`, `git`, `make`, and `python3`.
```
sudo apt install p7zip-full git make python3
```

*Source Code*

For HeliosSDK, please contact the [QuecPython technical team](https://python.quectel.com/en/contact) for the source code of HeliosSDK(We are preparing to open source), You can also get supports through email QuecPython@quectel.com.

For MicroPython, You can directly pull the official code, but you need to pull MicroPython into the heliossdk directory, for example, create a services directory under heliossdk and place MicroPython in the services directory.

Building the firmware
---------------------

Before you start building the firmware, you must build the MicroPython cross compiler firstly, it will be used to pre-compile some of the built-in scripts to bytecode.

> If you are in a Windows environment, you need a Windows compilation toolchain, such as MinGW.

```bash
# path: heliossdk/services/micropython
$ make -C mpy-cross
```

Then to build MicroPython for the quectel run:

```bash
# path: heliossdk/services/micropython
$ cd ports/quectel
$ make submodules
$ make
```

This will produce a combined `firmware.a` lib in the `build/`
subdirectory directly.The compiled MicroPython library will be used by HeliosSDK to build a complete firmware package. Of course, you can also build the entire firmware directly using the following method.

*Check the usage of the helios compilation commands*

In the command line started in the HeliosSDK directory, type `helios` and press "**Enter**" to view the usage of the helios command.
The output is as follows:

```
Usage: helios <action> [<app>] [<at>] [<fw_name>]

These are common commands used in various situations:
   menuconfig                       - Do the project configuration
   make <app> [[<at>] [<fw_name>]]  - Do the compilation work
   private_clean                    - Clean the app private target
   clean                            - Clean the output directory
   git [<args>]                     - Git commands execution
   help                             - Show this help page
```

> For detailed usage of the compilation command, please refer to  `README.MD` in the SDK root directory.

*Compile the firmware*

Taking the EG915UEC_AC module as an example, type the following command in the command line and press  "**Enter**":
```
helios make services/micropython @EG915UEC_AC EG915UECACR03A01M08
```

- `helios`: Trigger the compilation process.
- `make`: Compile the firmware.
- `services/micropython`: Application entry address (relative to the SDK root directory, according to the requirements of the host system, the Win32 platform is \ and the Linux platform is /). It can be adjusted according to the specific location of MicroPython.
- `@EG915UEC_AC`: Specify the target module model. You need to modify it according to your actual model.
- `EG915UECACR03A01M08`: Firmware version name, which can be omitted. You need to modify it according to your actual model.

*Check the compilation target*

The generated firmware package is stored in the *`output/release`* folder in the HeliosSDK root directory.

To clean the compilation target, type the following command in the command line and press "**Enter**":
```
helios clean
```

*Flash the firmware*

You need to use QPYcom or VSCode tool to burn the firmware. Please refer to [Quectel_QFlash_User_Guide](https://python.quectel.com/doc/Getting_started/en/flash_firmware.html) for firmware flashing.


Accessing the board
-------------------------------------

You can access via the `USB REPL` port, which stands for `Read-Eval-Print-Loop` (interactive interpreter). Please refer to [Quectel_Getting_Started](https://python.quectel.com/doc/Getting_started/en/REPL_dev.html) for firmware debugging.
