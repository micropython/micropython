.. _intro:

Getting started with MicroPython on the ESP8266
===============================================

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
converter to make the UART available to your PC.

MicroPython is distributed as several firmware variants to suit the amount of
flash on your board:

* The standard build (``ESP8266_GENERIC``) targets boards with **2MiB or more**
  of flash. This is the recommended build and the best choice for most users.
* The ``FLASH_1M`` variant is for boards with **1MiB** of flash. It removes
  asyncio and FAT-filesystem support, as well as some modules from
  micropython-lib.
* The ``FLASH_2M_ROMFS`` variant targets **2MiB** boards and reserves part of
  the flash for a read-only ROMFS filesystem.
* The ``FLASH_512K`` variant is for boards with only **512kiB** of flash. It is
  highly limited compared to the other builds: there is no filesystem support,
  and so features that depend on it won't work (WebREPL, mip, etc.). It also
  drops framebuffer support, some Python language features, and has less
  detailed error messages. This variant is mainly of interest to users who
  build from source and fine-tune parameters for their particular application.

The minimum recommended flash size is therefore 1MiB, with 2MiB or more giving
the best experience.

Names of pins will be given in this tutorial using the chip names (eg GPIO0)
and it should be straightforward to find which pin this corresponds to on your
particular board.

Powering the board
------------------

If your board has a USB connector on it then most likely it is powered through
this when connected to your PC.  Otherwise you will need to power it directly.
Please refer to the documentation for your board for further details.

Getting the firmware
--------------------

The first thing you need to do is download the most recent MicroPython firmware
.bin file to load onto your ESP8266 device. You can download it from the
`ESP8266 download page <https://micropython.org/download/ESP8266_GENERIC/>`_.

The download page offers the firmware variants described above. Pick the one
that matches your board's flash size:

========================================================= =========================================
Firmware file                                             Board flash size
========================================================= =========================================
``ESP8266_GENERIC-<date>-<version>.bin``                  2MiB-4MiB, 8MiB-16MiB** (the standard build)
``ESP8266_GENERIC-FLASH_1M-<date>-<version>.bin``         1MiB
``ESP8266_GENERIC-FLASH_2M_ROMFS-<date>-<version>.bin``   2MiB-4MiB, 8MiB-16MiB** (includes a ROMFS)
``ESP8266_GENERIC-FLASH_512K-<date>-<version>.bin``       512kiB
========================================================= =========================================

** Boards with 8MiB or 16MiB of flash can use the standard build, 
but require an manual step to set up the RF calibration data, see :ref:`esp8266_large_flash` below.

For each variant the page lists *release* builds and *preview* builds. If you
are just starting with MicroPython, choose the latest release build. If you are
an experienced user who would like to follow development closely and help with
testing new features, the preview builds are automatic builds of the
development branch.

Throughout the rest of this tutorial the example commands use the standard
``ESP8266_GENERIC`` firmware; substitute the exact filename you downloaded.

Deploying the firmware
----------------------

Once you have the MicroPython firmware (compiled code), you need to load it onto
your ESP8266 device.  There are two main steps to do this: first you
need to put your device in boot-loader mode, and second you need to copy across
the firmware.  The exact procedure for these steps is highly dependent on the
particular board and you will need to refer to its documentation for details.

If you have a board that has a USB connector, a USB-serial converter, and has
the DTR and RTS pins wired in a special way then deploying the firmware should
be easy as all steps can be done automatically.  Boards that have such features
include the Adafruit Feather HUZZAH and NodeMCU boards.

If you do not have such a board, you need keep GPIO0 pulled to ground and reset
the device by pulling the reset pin to ground and releasing it again to enter
programming mode.

For best results it is recommended to first erase the entire flash of your
device before putting on new MicroPython firmware.

Currently we only support esptool.py to copy across the firmware.  You can find
this tool here: `<https://github.com/espressif/esptool/>`__, or install it
using pip::

    pip install esptool

Any other flashing program should work, so feel free to try them out or refer
to the documentation for your board to see its recommendations.

Using esptool.py you can erase the flash with the command::

    esptool.py --port /dev/ttyUSB0 erase_flash

And then deploy the new firmware using::

    esptool.py --port /dev/ttyUSB0 --baud 460800 write_flash --flash_size=detect 0 ESP8266_GENERIC-20260406-v1.28.0.bin

You might need to change the "port" setting to something else relevant for your
PC.  You may also need to reduce the baudrate if you get errors when flashing
(eg down to 115200).  The filename of the firmware should also match the file
that you have.

The ``--flash_size=detect`` option tells esptool.py to read the flash size from
the chip's JEDEC ID. MicroPython itself also autodetects the flash size at
runtime for chips up to **4MB**, so a single firmware build adapts to the actual
flash on your board without any extra configuration. The filesystem is
automatically sized to use all of the available flash.

The ESP8266 needs a small block of RF calibration data, known as
``esp_init_data``, near the end of the flash before WiFi will start. On first
boot MicroPython checks this region and, if it is blank (for example because you
just ran ``erase_flash``), it automatically writes the default calibration data
for you. For boards with **4MB of flash or less**, no manual step is needed. See
:ref:`esp8266_large_flash` below for boards larger than 4MB.

For some boards with a particular FlashROM configuration (e.g. some variants of
a NodeMCU board) you may need to manually set a compatible
`SPI Flash Mode <https://github.com/espressif/esptool/wiki/SPI-Flash-Modes>`_.
You'd usually pick the fastest option that is compatible with your device, but
the ``-fm dout`` option (the slowest option) should have the best compatibility::

    esptool.py --port /dev/ttyUSB0 --baud 460800 write_flash --flash_size=detect -fm dout 0 ESP8266_GENERIC-20260406-v1.28.0.bin

If the above commands run without error then MicroPython should be installed on
your board!

If you pulled GPIO0 manually to ground to enter programming mode, release it
now and reset the device by again pulling the reset pin to ground for a short
duration.

.. _esp8266_large_flash:

Boards with more than 4MB of flash
----------------------------------

Boards with **8MB or 16MB** of flash need one extra step. The flash routines
built into the ESP8266 boot ROM (which MicroPython uses to write the calibration
data on first boot) cannot address flash beyond 4MB: the ROM reads the chip's
device ID but clamps the reported size to 4MB, so any access above that offset
fails. As a result MicroPython can only write the ``esp_init_data`` RF
calibration block automatically within the first 4MB of flash.
So after a full flash erase MicroPython cannot place the calibration data at the correct end-of-flash
address by itself. Without it the WiFi subsystem will not start (typically
showing up as a continuous reset loop or ``rf_cal`` errors). For more background
on this ROM limitation see
`ESP8266 16MB Flash Handling <https://piers.rocks/esp8266/16mb/flash/eeprom/2016/10/14/esp8266-16mbyte-flash_handling.html>`_.

To fix this, flash the ``esp_init_data_default.bin`` file (shipped with the
Espressif NONOS SDK) to the calibration address, which is the flash size minus
``0x4000``, see table below.

You can download ``esp_init_data_default.bin`` from the Espressif repository
(open the link and click "View raw" to download the file):
`<https://github.com/espressif/ESP8266_AT/blob/master/bin/esp_init_data_default.bin>`__

The calibration addresses are:

=========== =====================================
Flash size  ``esp_init_data`` address
=========== =====================================
8MB         ``0x7FC000``
16MB        ``0xFFC000``
=========== =====================================

The full procedure for a 16MB board is::

    # 1. Make sure esptool is up to date
    pip install --upgrade esptool

    # 2. Erase the flash
    esptool.py --port /dev/ttyUSB0 --baud 460800 erase_flash

    # 3. Flash the RF calibration blob at the end of flash (16MB example)
    esptool.py --port /dev/ttyUSB0 write_flash 0xFFC000 esp_init_data_default.bin

    # 4. Flash MicroPython, telling esptool the real flash size
    esptool.py --port /dev/ttyUSB0 --baud 460800 \
        write_flash -fm dio --flash_size 16MB 0 ESP8266_GENERIC-20260406-v1.28.0.bin

    # 5. Check detected flash size in the REPL
    mpremote exec "import esp;print(f'Detected flash: {esp.flash_size():_}')"

For an 8MB board, use ``0x7FC000`` in step 3 and ``--flash_size 8MB`` in step 4.

The filesystem is sized automatically from the detected flash size, so it
will use all of the available space once the board boots.


Serial prompt
-------------

Once you have the firmware on the device you can access the REPL (Python prompt)
over UART0 (GPIO1=TX, GPIO3=RX), which might be connected to a USB-serial
converter, depending on your board.  The baudrate is 115200.  The next part of
the tutorial will discuss the prompt in more detail.

WiFi
----

After a fresh install and boot the device configures itself as a WiFi access
point (AP) that you can connect to.  The ESSID is of the form MicroPython-xxxxxx
where the x's are replaced with part of the MAC address of your device (so will
be the same every time, and most likely different for all ESP8266 chips).  The
password for the WiFi is micropythoN (note the upper-case N).  Its IP address
will be 192.168.4.1 once you connect to its network.  WiFi configuration will
be discussed in more detail later in the tutorial.

Troubleshooting installation problems
-------------------------------------

If you experience problems during flashing or with running firmware immediately
after it, here are troubleshooting recommendations:

* Be aware of and try to exclude hardware problems. There are 2 common problems:
  bad power source quality and worn-out/defective FlashROM. Speaking of power
  source, not just raw amperage is important, but also low ripple and noise/EMI
  in general. If you experience issues with self-made or wall-wart style power
  supply, try USB power from a computer. Unearthed power supplies are also known
  to cause problems as they source of increased EMI (electromagnetic interference)
  - at the very least, and may lead to electrical devices breakdown. So, you are
  advised to avoid using unearthed power connections when working with ESP8266
  and other boards. In regard to FlashROM hardware problems, there are independent
  (not related to MicroPython in any way) reports
  `(e.g.) <http://internetofhomethings.com/homethings/?p=538>`_
  that on some ESP8266 modules, FlashROM can be programmed as little as 20 times
  before programming errors occur. This is *much* less than 100,000 programming
  cycles cited for FlashROM chips of a type used with ESP8266 by reputable
  vendors, which points to either production rejects, or second-hand worn-out
  flash chips to be used on some (apparently cheap) modules/boards. You may want
  to use your best judgement about source, price, documentation, warranty,
  post-sales support for the modules/boards you purchase.

* The flashing instructions above use flashing speed of 460800 baud, which is
  good compromise between speed and stability. However, depending on your
  module/board, USB-UART converter, cables, host OS, etc., the above baud
  rate may be too high and lead to errors. Try a more common 115200 baud
  rate instead in such cases.

* If lower baud rate didn't help, you may want to try a different version of
  esptool.py, which may use a different programming algorithm.

* The ``--flash_size`` option in the commands above is mandatory. Omitting
  it will lead to a corrupted firmware.

* To catch incorrect flash content (e.g. from a defective sector on a chip),
  add ``--verify`` switch to the commands above.

* Additionally, you can check the firmware integrity from a MicroPython REPL
  prompt (assuming you were able to flash it and ``--verify`` option doesn't
  report errors)::

    import esp
    esp.check_fw()

  If the last output value is True, the firmware is OK. Otherwise, it's
  corrupted and need to be reflashed correctly.

* If you experience any issues with another flashing application (not
  esptool.py), try esptool.py, it is a generally accepted flashing
  application in the ESP8266 community.

* If you still experience problems with even flashing the firmware, please
  refer to esptool.py project page, https://github.com/espressif/esptool
  for additional documentation and bug tracker where you can report problems.

* If you are able to flash firmware, but ``--verify`` option or
  ``esp.check_fw()`` return errors even after multiple retries, you
  may have a defective FlashROM chip, as explained above.
