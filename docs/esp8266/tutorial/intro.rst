Introduction to MicroPython on the ESP8266
==========================================

Using MicroPython is a great way to get the most of your ESP8266 board.  And
vice versa, the ESP8266 chip is a great platform for using MicroPython.  This
tutorial will guide you through setting up MicroPython, getting a prompt, using
WebREPL, connecting to the network and communicating with the Internet, using
the hardware peripherals, and controlling some external components.

Let's get started!

Requirements
------------

The first thing you need is a board with an ESP8266 chip.  The MicroPython
software supports the ESP8266 chip itself and any board should work.  The main
characteristic of a board is how much flash it has, how the GPIO pins are
connected to the outside world, and whether it includes a built-in USB-serial
convertor to make the UART available to your PC.

The minimum requirement for flash size is 512k.  A board with this amount of
flash will not have room for a filesystem, but otherwise is fully functional.
If your board has 1Mbyte or more of flash then it will support a filesystem.

Names of pins will be given in this tutorial using the chip names (eg GPIO0)
and it should be straightforward to find which pin this corresponds to on your
particular board.

Powering the board
------------------

If your board has a USB connector on it then most likely it is powered through
this when connected to your PC.  Otherwise you will need to power it directly.
Please refer to the documentation for your board for further details.

Deploying the firmware
----------------------

The very first thing you need to do is put the MicroPython firmware (compiled
code) on your ESP8266 device.  There are two main steps to do this: first you
need to put your device in boot-loader mode, and second you need to copy across
the firmware.  The exact procedure for these steps is highly dependent on the
particular board and you will need to refer to its documentation for details.

If you have a board that has a USB connector, a USB-serial convertor, and has
the DTR and RTS pins wired in a special way then deploying the firmware should
be easy as all steps can be done automatically.  Boards that have such features
include the Adafruit Feather HUZZAH and NodeMCU boards.

For best results it is recommended to first erase the entire flash of your
device before putting on new MicroPython firmware.

Currently we only support esptool.py to copy across the firmware.  You can find
this tool here: `<https://github.com/themadinventor/esptool/>`__, or install it
using pip::

    pip install esptool

It requires Python 2.7, so you may need to use ``pip2`` instead of ``pip`` in
the command above.  Any other
flashing program should work, so feel free to try them out, or refer to the
documentation for your board to see its recommendations.

Using esptool.py you can erase the flash with the command::

    esptool.py --port /dev/ttyUSB0 erase_flash

And then deploy the new firmware using::

    esptool.py --port /dev/ttyUSB0 --baud 460800 write_flash --flash_size=8m 0 esp8266-2016-05-03-v1.8.bin

You might need to change the "port" setting to something else relevant for your
PC.  You may also need to reduce the baudrate if you get errors when flashing
(eg down to 115200).  The filename of the firmware should also match the file
that you have.

If you have a NodeMCU board, you may need to use the following command to deploy
the firmware (note the "-fm dio" option)::

    esptool.py --port /dev/ttyUSB0 --baud 460800 write_flash --flash_size=8m -fm dio 0 esp8266-2016-05-03-v1.8.bin

If the above commands run without error then MicroPython should be installed on
your board!

Serial prompt
-------------

Once you have the firmware on the device you can access the REPL (Python prompt)
over UART0 (GPIO1=TX, GPIO3=RX), which might be connected to a USB-serial
convertor, depending on your board.  The baudrate is 115200.  The next part of
the tutorial will discuss the prompt in more detail.

WiFi
----

After a fresh install and boot the device configures itself as a WiFi access
point (AP) that you can connect to.  The ESSID is of the form MicroPython-xxxxxx
where the x's are replaced with part of the MAC address of your device (so will
be the same everytime, and most likely different for all ESP8266 chips).  The
password for the WiFi is micropythoN (note the upper-case N).  Its IP address
will be 192.168.4.1 once you connect to its network.  WiFi configuration will
be discussed in more detail later in the tutorial.
