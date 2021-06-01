.. _rp2_intro:

1.Getting started with MicroPython on the RP2xxx
================================================

The Pico board is the most popular in the series and we will be talking about it in general for all the RP2xxx boards.This tutorial will guide you through setting up MicroPython, connecting to the network and communicating with the Internet, using the hardware peripherals, and controlling some external components.

1.1. The MicroPython Environment
---------------------------------

Python is the fastest way to get started with embedded software on RP2xxx ports. Because MicroPython is highly efficient, and RP2040 is designed with a disproportionate amount of system memory and processing power for its price, MicroPython is a serious tool for embedded systems development, which does not compromise on approachability.The accelerated floating point libraries in RP2040’s on-board ROM storage are used automatically by your Python code, so you should find arithmetic performance quite snappy.


Most on-chip hardware is exposed through the standard *machine* module, so existing MicroPython projects can be ported without too much trouble. The second processor core is exposed through the *_thread* module.

RP2040 has some unique hardware you won’t find on other microcontrollers, with the programmable I/O system (PIO) being the prime example of this: a versatile hardware subsystem that lets you create new I/O interfaces and run them at high speed. In the rp2 module you will find a comprehensive PIO library which lets you write new PIO programs at the MicroPython prompt, and interact with them in real time, to develop interfaces for new or unusual pieces of hardware (or indeed if you just find yourself wanting an extra few serial ports).

MicroPython implements the entire Python 3.4 syntax (including exceptions, with, yield from, etc., and additionally *async /await* keywords from Python 3.5). The following core datatypes are provided: *str (including basic Unicode support), bytes, bytearray, tuple, list, dict, set, frozenset, array.array, collections.namedtuple,* classes and instances. Builtin modules include sys, time, and struct, etc. Note that only a subset of Python 3 functionality is implemented for the data types and modules.MicroPython can execute scripts in textual source form (*.py* files) or from precompiled bytecode, in both cases either from an on-device filesystem or "frozen" into the MicroPython executable.

1.2. Requirements
------------------

The first thing you need is a board with a RP2040 chip. A number of manufacturers have announced their own boards using RP2xxx and Raspberri Pi Pico will d just fine for discussion of an example.The key features of the chip include 133Mhz dual ARM Cortex-MO+ cores(overcloackable to over 400 MHz) with each core having an integer divider peripheral and two interpolators, 264 KB SRAM in six independent banks. no internal Flash memory (after reset, the boot-loader loads firmware from either external flash memory or USB bus into internal SRAM),QSPI bus controller, supporting up to 16 MB of external Flash memory, DMA controller, AHB crossbar, fully-connected, On-chip programmable LDO to generate core voltage, 2 on-chip PLLs to generate USB and core clocks, 30 GPIO pins, of which 4 can optionally be used as analog inputs. The peripherals include 2 UARTs, 2 SPI controllers, 2 I2C controllers, 16 PWM channels, USSB 1.1 controller and PHY, with host and device support and 8 PIO state machines.

1.3. Powering the board
------------------------

If your board has a USB connector on it then most likely it is powered through this when connected to your PC. Otherwise you will need to power it directly. Please refer to the documentation for your board for further details.

1.4. Getting the firmware
--------------------------

The fastest way to get MicroPython is to download the pre-built release binary from the `Pico Getting Started pages https://www.raspberrypi.org/documentation/rp2040/getting-started/`_. If you can’t or don’t want to use the pre-built release — for example, if you want to develop a C module for MicroPython — you can follow section 1.4 to  get the source code for MicroPython, which you can use to build your own MicroPython firmware binary.

1.5. Installing MicroPython on Raspberry Pi Pico
-------------------------------------------------

Raspberry Pi Pico has a BOOTSEL mode for programming firmware over the USB port. Holding the BOOTSEL button when powering up your board will put it into a special mode where it appears as a USB Mass Storage Device. First make sure your Raspberry Pi Pico is not plugged into any source of power: disconnect the micro USB cable if plugged in, and disconnect any other wires that might be providing power to the board, e.g. through the VSYS or VBUS pin. Now hold down the BOOTSEL button, and plug in the micro USB cable (which hopefully has the other end plugged into your computer).

A drive called RPI-RP2 should pop up. Go ahead and drag the MicroPython *firmware.uf2* file onto this drive. This programs the MicroPython firmware onto the flash memory on your Raspberry Pi Pico.
It should take a few seconds to program the UF2 file into the flash. The board will automatically reboot when finished,causing the RPI-RP2 drive to disappear, and boot into MicroPython.
By default, MicroPython doesn’t do anything when it first boots. It sits and waits for you to type in further instructions.

The `Getting started with Raspberry Pi Pico https://datasheets.raspberrypi.org/pico/getting-started-with-pico.pdf`_ book has detailed instructions on getting your Raspberry Pi Pico into BOOTSEL mode and loading UF2 files, in case you are having trouble. There is also a section going over loading ELF files with the debugger, in case your board doesn’t have an easy way of entering BOOTSEL, or you would like to debug a MicroPython C module you are developing.

.. note:: 

	If you are not following these instructions on a Raspberry Pi Pico, you may not have a BOOTSEL 	button. If this is the case, you should check if there is some other way of grounding the flash CS pin, such as a jumper, to tell RP2040 to enter the BOOTSEL mode on boot. If there is no such method, you can load code using the Serial Wire Debug interface.

1.6. Building MicroPython from Source
--------------------------------------

The prebuilt binaries on the Pico Getting Started pages should serve most use cases, but you can build your own MicroPython firmware from source if you’d like to customise its low-level aspects.

If you  have already downloaded and installed a prebuilt MicroPython UF2 file, you can skip ahead to the next section to start using your board.

These instructions for getting and building MicroPython assume you are using Raspberry Pi OS running on a
Raspberry Pi 4, or an equivalent Debian-based Linux distribution running on another platform.

It’s a good idea to create a pico directory to keep all pico-related checkouts in. These instructions create a pico directory at */home/pi/pico.*

.. code-block:: bash
	
	$ cd ~/
	$ mkdir pico
	$ cd pico

Then clone the *micropython* git repository. These instructions will fetch the latest version of the source code.

.. code-block:: bash

	$ git clone -b master https://github.com/micropython/micropython.git

Once the download has finished, the source code for MicroPython should be in a new directory called micropython. The MicroPython repository also contains pointers (submodules) to specific versions of libraries it needs to run on a particular board, like the SDK in the case of RP2040. We need to explicitly fetch these too:

.. code-block:: bash

	$ cd micropython
	$ git submodule update --init -- lib/pico-sdk lib/tinyusb

.. note:: 

	The following instructions assume that you are using a Raspberry Pi Pico. Some details may differ if you are building firmware for a different RP2040-based board. The board vendor should detail any extra 	    steps needed to build firmware for that particular board. The version we’re building here is fairly generic, but there might be some differences like putting the default serial port on different pins,or 		including extra modules to drive that board’s hardware.

To build the RP2040 MicroPython port, you’ll need to install some extra tools. To build projects you’ll need `CMake https://cmake.org/`_ , a cross-platform tool used to build the software, and the `GNU Embedded Toolchain for Arm https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads`_, which turns MicroPython’s C source code into a binary program RP2040’s processors can understand. *build-essential* is a bundle of tools you need to build code native to your own machine — this is needed for some internal tools in MicroPython and the SDK. You can install all of these via apt from the command line. Anything you already have installed will be ignored by *apt.*

.. code-block:: bash

	$ sudo apt update
	$ sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential

First we need to bootstrap a special tool for MicroPython builds, that ships with the source code:

.. code-block:: bash

	$ make -C mpy-cross

We can now build the port we need for RP2040, that is, the version of MicroPython that has specific support for our chip.

.. code-block:: bash

	$ cd portal/rp2
	$ make

If everything went well, there will be a new directory called *build* ( *ports/rp2/build* relative to the micropython directory), which contains the new firmware binaries. The most important ones are:

**firmware.uf2** : A UF2 binary file which can dragged onto the RPI-RP2 drive that pops up once your Raspberry Pi Pico is in BOOTSEL mode. The firmware binaries you will find on Pico Getting Started pages are UF2 files, because they’re the easiest to install.

**firmware.elf** : A different type of binary file, which can be loaded by a debugger (such as *gdb* with *openocd*) over RP2040’s SWD debug port. This is useful for debugging either a native C module you’ve added to MicroPython, or the MicroPython core interpreter itself. The actual binary contents is the same
as *firmware.uf2*.

.. seealso::

	You can take a look inside your new firmware.uf2 using picotool, see the Appendix B in the Getting started with Raspberry Pi Pico book for details of how to use picotool.

2.2 Connecting to the MicroPython REPL
======================================

When MicroPython boots for the first time, it will sit and wait for you to connect and tell it what to do. You can load a *.py* file from your computer onto the board, but a more immediate way to interact with it is through what is called the readevaluate-print loop, or REPL (often pronounced similarly to "ripple").

**Read** - MicroPython waits for you to type in some text, followed by the enter key.

**Evaluate** -  Whatever you typed is interpreted as Python code, and runs immediately.

**Print** - Any results of the last line you typed are printed out for you to read.

**Loop** - Go back to the start — prompt you for another line of code.

There are two ways to connect to this REPL, so you can communicate with the MicroPython firmware on your board: over USB, and over the UART serial port on Raspberry Pi Pico GPIOs.

2.1. Connecting from a Raspberry Pi over USB
---------------------------------------------

The MicroPython firmware is equipped with a virtual USB serial port which is accessed through the micro USB connector on Raspberry Pi Pico. Your computer should notice this serial port and list it as a character device, most likely */dev/ttyACM0.*

.. note::

	You can run ls /dev/tty* to list your serial ports. There may be quite a few, but MicroPython’s 	USB serial will start with /dev/ttyACM. If in doubt, unplug the micro USB connector and see which 		one disappears. If you don’t see anything, you can try rebooting your Raspberry Pi.


You can install *minicom* to access the serial port:

.. code-block:: bash

	$ sudo apt install minicom

and then open it as such:

.. code-block:: bash

	$ minicom -o -D /dev//ttyACM0

Where the *-D /dev/ttyACM0* is pointing *minicom* at MicroPython’s USB serial port, and the -o flag essentially means "just do it". There’s no need to worry about baud rate, since this is a virtual serial port. Press the enter key a few times in the terminal where you opened *minicom.* You should see this:

>>>

This is a prompt. MicroPython wants you to type something in, and tell it what to do.
If you press CTRL-D on your keyboard whilst the minicom terminal is focused, you should see a message similar to this:

.. topic:: Message

	MPY: soft reboot
	MicroPython v1.13-422-g904433073 on 2021-01-19; Raspberry Pi Pico with RP2040
	Type "help()" for more information.
	>>>

This key combination tells MicroPython to reboot. You can do this at any time. When it reboots, MicroPython will printout a message saying exactly what firmware version it is running, and when it was built. Your version number will be different from the one shown here.

2.2. Connecting from a Raspberry Pi Pico using UART
---------------------------------------------------

.. warning::

	REPL over UART is disabled by default.

The MicroPython port for RP2040 does not expose REPL over a UART port by default. However this default can bechanged in the ports/rp2/mpconfigport.h source file. If you want to use the REPL over UART you’re going to have to build MicroPython yourself, see section 1.6 for details.

Go ahead and download the MicroPython source and in *ports/rp2/mpconfigport.h* change *MICROPY_HW_ENABLE_UART_REPL* to 1 to enable it.

.. code-block:: python3

	#define MICROPY_HW_ENABLE_UART_REPL (1) // useful if there is no USB

Then continue to follow the instructions in Section 1.6 to build your own MicroPython UF2 firmware.

This will allow the REPL to be accessed over a UART port, through two GPIO pins. The default settings for UARTs are taken from the C SDK.

       =============       ========= 
	Function            Default
       =============       =========
       UART_BAUDRATE        115200
       UART_BTS             8
       UART_STOP            1
       UART0_TX             Pin 0
       UART0_RX             Pin 1
       UART1_TX             Pin 4
       UART1_RX             Pin 5
       =============       =========

This table shows the default UART settings.

This alternative interface is handy if you have trouble with USB, if you don’t have any free USB ports, or if you are using some other RP2040-based board which doesn’t have an exposed USB connector. 


.. note:: 

	This initially occupies the *UART0* peripheral on RP2040. The *UART1* peripheral is free for you to use in your Python code as a second UART.

The next thing you’ll need to do is to enable UART serial on the Raspberry Pi. To do so, run *raspi-config*

.. code-block:: bash

	$ sudo raspi-config

and go to *Interfacing Options → Serial* and select "No" when asked "Would you like a login shell to be accessible over serial?" and "Yes" when asked "Would you like the serial port hardware to be enabled?".You should see something like this.

.. image:: fig1.jpeg
    :width: 200px
    :align: center
    :height: 100px
    :alt: alternate text


Leaving *raspi-config* you should choose "Yes" and reboot your Raspberry Pi to enable the serial port.
You should then wire the Raspberry Pi and the Raspberry Pi Pico together with the following mapping:

	=================           ==================
        Raspberry Pi                Raspberry Pi Pico
        =================           ==================
        GND                         GND
        GPIO15(UART_RX0)            GPIO0(UART_TX)
        GPIO14(UART_TX0)            GPIO1(UART_RX)
        =================           ==================

.. warning:: 

	RX matches to TX, and TX matches to RX. You mustn’t connect the two opposite TX pins together, or the two RX pins. This is because MicroPython needs to listen on the channel that the Raspberry Pi 		transmits on, and vice versa.

then connect to the board using minicom connected to */dev/serial0*

$ minicom -b 115200 -o -D /dev/serial0

If you press the enter key, MicroPython should respond by prompting you for more input:

>>>

	







