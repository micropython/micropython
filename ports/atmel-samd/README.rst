SAMD21x18
=========

This port brings MicroPython to SAMD21x18 based development boards under the name
CircuitPython. Supported boards include the Adafruit CircuitPlayground Express,
Adafruit Feather M0 Express, Adafruit Metro M0 Express, Arduino Zero, Adafruit
Feather M0 Basic and Adafruit M0 Bluefruit LE.

Pinout
------

All of the boards share the same core pin functionality but call pins by
different names. The table below matches the pin order in
`the datasheet <http://ww1.microchip.com/downloads/en/DeviceDoc/40001882A.pdf>`_
and omits the pins only available on the largest package because all supported
boards use smaller version.

=====================  ===============  ===========================  ====================  ================  ==================  =========================  ================  ================
`microcontroller.pin`  `board`
---------------------  -----------------------------------------------------------------------------------------------------------------------------------------------------------------------
Datasheet              arduino_zero     circuitplayground_express    feather_m0_adalogger  feather_m0_basic  feather_m0_express  gemma_m0                   metro_m0_express  trinket_m0
=====================  ===============  ===========================  ====================  ================  ==================  =========================  ================  ================
PA00                                    ``ACCELEROMETER_SDA``                                                                    ``APA102_MOSI``                              ``APA102_MOSI``
PA01                                    ``ACCELEROMETER_SCL``                                                                    ``APA102_SCK``                               ``APA102_SCK``
PA02                   ``A0``           ``A0`` / ``SPEAKER``         ``A0``                ``A0``            ``A0``              ``A0`` / ``D1``            ``A0``            ``D1`` / ``A0``
PA03
PB08                   ``A1``           ``A7`` / ``TX``              ``A1``                ``A1``            ``A1``                                         ``A1``
PB09                   ``A2``           ``A6`` / ``RX``              ``A2``                ``A2``            ``A2``                                         ``A2``
PA04                   ``A3``           ``IR_PROXIMITY``             ``A3``                ``A3``            ``A3``              ``D0`` / ``TX`` / ``SDA``  ``A3``
PA05                   ``A4``           ``A1``                       ``A4``                ``A4``            ``A4``              ``D2`` / ``RX`` / ``SCL``  ``A4``
PA06                   ``D8``           ``A2``                                                               ``NEOPIXEL``                                   ``D8``            ``D4`` / ``TX``
PA07                   ``D9``           ``A3``                       ``D9``                ``D9``            ``D9``                                         ``D9``            ``D3`` / ``RX``
PA08                   ``D4``           ``MICROPHONE_DO``            ``SD_CS``                                                                              ``D4``            ``D0`` / ``SDA``
PA09                   ``D3``           ``TEMPERATURE`` / ``A9``                                                                                            ``D3``            ``D2`` / ``SCL``
PA10                   ``D1`` / ``TX``  ``MICROPHONE_SCK``           ``D1`` / ``TX``       ``D1`` / ``TX``   ``D1`` / ``TX``                                ``D1`` / ``TX``   ``D13``
PA11                   ``D0`` / ``RX``  ``LIGHT`` / ``A8``           ``D0`` / ``RX``       ``D0`` / ``RX``   ``D0`` / ``RX``                                ``D0`` / ``RX``
PB10                   ``MOSI``                                      ``MOSI``              ``MOSI``          ``MOSI``                                       ``MOSI``
PB11                   ``SCK``                                       ``SCK``               ``SCK``           ``SCK``                                        ``SCK``
PA12                   ``MISO``         ``REMOTEIN`` / ``IR_RX``     ``MISO``              ``MISO``          ``MISO``                                       ``MISO``
PA13                                    ``ACCELEROMETER_INTERRUPT``                                                                                         ``FLASH_CS``
PA14                   ``D2``           ``BUTTON_B`` / ``D5``                                                                                               ``D2``
PA15                   ``D5``           ``SLIDE_SWITCH`` / ``D7``    ``D5``                ``D5``            ``D5``                                         ``D5``
PA16                   ``D11``          ``MISO``                     ``D11``               ``D11``           ``D11``                                        ``D11``
PA17                   ``D13``          ``D13``                      ``D13``               ``D13``           ``D13``                                        ``D13``
PA18                   ``D10``                                       ``D10``               ``D10``           ``D10``                                        ``D10``
PA19                   ``D12``                                       ``D12``               ``D12``           ``D12``                                        ``D12``
PA20                   ``D6``           ``MOSI``                     ``D6``                ``D6``            ``D6``                                         ``D6``
PA21                   ``D7``           ``SCK``                                                                                                             ``D7``
PA22                   ``SDA``                                       ``SDA``               ``SDA``           ``SDA``                                        ``SDA``
PA23                   ``SCL``          ``REMOTEOUT`` / ``IR_TX``    ``SCL``               ``SCL``           ``SCL``             ``L`` / ``D13``            ``SCL``
PA24
PA25
PB22                                    ``FLASH_CS``
PB23                                    ``NEOPIXEL`` / ``D8``
PA27
PA28                                    ``BUTTON_A`` / ``D4``
PA29
PA30                                    ``SPEAKER_ENABLE``                                                                                                  ``NEOPIXEL``
PA31
PB02                   ``A5``           ``A5`` / ``SDA``             ``A5``                ``A5``            ``A5``                                         ``A5``
PB03                                    ``A4`` / ``SCL``
=====================  ===============  ===========================  ====================  ================  ==================  =========================  ================  ================

Here is a table about which pins can do what in CircuitPython terms. However,
just because something is listed, doesn't mean it will always work. Existing use
of other pins and functionality will impact your ability to use a pin for your
desired purpose. For example, only certain combinations of SPI pins will work
because they use shared hardware internally.

=====================  ========  =========  =========  =======  =======  =======  =========  =========  =======  ==========  ==========  =========  =========  =========  ============  =======  =======  =========
`microcontroller.pin`  `analogio`           `audioio`  `bitbangio`                `busio`                                                                                 `digitalio`   `pulseio`         `touchio`
---------------------  -------------------  ---------  -------------------------  --------------------------------------------------------------------------------------  ------------  ----------------  ---------
Datasheet              AnalogIn  AnalogOut  AudioOut   I2C      OneWire  SPI      I2C - SDA  I2C - SCL  OneWire  SPI - MISO  SPI - MOSI  SPI - SCK  UART - RX  UART - TX  DigitalInOut  PulseIn  PWMOut   TouchIn
=====================  ========  =========  =========  =======  =======  =======  =========  =========  =======  ==========  ==========  =========  =========  =========  ============  =======  =======  =========
PA00                                                   **Yes**  **Yes**  **Yes**  **Yes**               **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**
PA01                                                   **Yes**  **Yes**  **Yes**             **Yes**    **Yes**  **Yes**                 **Yes**    **Yes**               **Yes**       **Yes**  **Yes**
PA02                   **Yes**   **Yes**    **Yes**    **Yes**  **Yes**  **Yes**                        **Yes**                                                           **Yes**       **Yes**           **Yes**
PA03                   **Yes**                         **Yes**  **Yes**  **Yes**                        **Yes**                                                           **Yes**       **Yes**           **Yes**
PB08                   **Yes**                         **Yes**  **Yes**  **Yes**  **Yes**               **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**           **Yes**
PB09                   **Yes**                         **Yes**  **Yes**  **Yes**             **Yes**    **Yes**  **Yes**                 **Yes**    **Yes**               **Yes**       **Yes**  **Yes**  **Yes**
PA04                   **Yes**                         **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**  **Yes**  **Yes**
PA05                   **Yes**                         **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**                 **Yes**    **Yes**               **Yes**       **Yes**  **Yes**  **Yes**
PA06                   **Yes**                         **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**  **Yes**  **Yes**
PA07                   **Yes**                         **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**                 **Yes**    **Yes**               **Yes**       **Yes**  **Yes**  **Yes**
PA08                   **Yes**                         **Yes**  **Yes**  **Yes**  **Yes**               **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**  **Yes**
PA09                   **Yes**                         **Yes**  **Yes**  **Yes**             **Yes**    **Yes**  **Yes**                 **Yes**    **Yes**               **Yes**       **Yes**  **Yes**
PA10                   **Yes**                         **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**  **Yes**
PA11                   **Yes**                         **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**     **Yes**    **Yes**               **Yes**       **Yes**  **Yes**
PB10                                                   **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**  **Yes**
PB11                                                   **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**     **Yes**    **Yes**               **Yes**       **Yes**  **Yes**
PA12                                                   **Yes**  **Yes**  **Yes**  **Yes**               **Yes**  **Yes**                            **Yes**    **Yes**    **Yes**       **Yes**  **Yes**
PA13                                                   **Yes**  **Yes**  **Yes**             **Yes**    **Yes**  **Yes**                 **Yes**    **Yes**               **Yes**       **Yes**  **Yes**
PA14                                                   **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**  **Yes**
PA15                                                   **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**     **Yes**    **Yes**               **Yes**       **Yes**  **Yes**
PA16                                                   **Yes**  **Yes**  **Yes**  **Yes**               **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**  **Yes**
PA17                                                   **Yes**  **Yes**  **Yes**             **Yes**    **Yes**  **Yes**                 **Yes**    **Yes**               **Yes**       **Yes**  **Yes**
PA18                                                   **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**  **Yes**
PA19                                                   **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**     **Yes**    **Yes**               **Yes**       **Yes**  **Yes**
PA20                                                   **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**  **Yes**
PA21                                                   **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**     **Yes**    **Yes**               **Yes**       **Yes**  **Yes**
PA22                                                   **Yes**  **Yes**  **Yes**  **Yes**               **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**  **Yes**
PA23                                                   **Yes**  **Yes**  **Yes**             **Yes**    **Yes**  **Yes**                 **Yes**    **Yes**               **Yes**       **Yes**  **Yes**
PA24
PA25
PB22                                                   **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**
PB23                                                   **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**     **Yes**    **Yes**               **Yes**       **Yes**
PA27                                                   **Yes**  **Yes**  **Yes**                        **Yes**                                                           **Yes**       **Yes**
PA28                                                   **Yes**  **Yes**  **Yes**                        **Yes**                                                           **Yes**       **Yes**
PA29                                                   **Yes**  **Yes**  **Yes**                        **Yes**                                                           **Yes**       **Yes**
PA30                                                   **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**  **Yes**
PA31                                                   **Yes**  **Yes**  **Yes**                        **Yes**  **Yes**     **Yes**     **Yes**    **Yes**               **Yes**       **Yes**  **Yes**
PB02                   **Yes**                         **Yes**  **Yes**  **Yes**  **Yes**               **Yes**  **Yes**     **Yes**                **Yes**    **Yes**    **Yes**       **Yes**           **Yes**
PB03                   **Yes**                         **Yes**  **Yes**  **Yes**             **Yes**    **Yes**  **Yes**                 **Yes**    **Yes**               **Yes**       **Yes**           **Yes**
=====================  ========  =========  =========  =======  =======  =======  =========  =========  =======  ==========  ==========  =========  =========  =========  ============  =======  =======  =========

Building
--------

To build for the Arduino Zero:

.. code-block:: shell

    make

To build for other boards you must change it by setting ``BOARD``. For example:

.. code-block:: shell

    make BOARD=feather_m0_basic

Board names are the directory names in the `boards <https://github.com/adafruit/circuitpython/tree/master/atmel-samd/boards>`_ folder.

Deploying
---------

Arduino Bootloader
^^^^^^^^^^^^^^^^^^

If your board has an existing Arduino bootloader on it then you can use bossac
to flash MicroPython. First, activate the bootloader. On Adafruit Feathers you
can double click the reset button and the #13 will fade in and out. Finally,
run bossac:

    tools/bossac_osx -e -w -v -b -R build-feather_m0_basic/firmware.bin

No Bootloader via GDB
^^^^^^^^^^^^^^^^^^^^^

This method works for loading MicroPython onto the Arduino Zero via the
programming port rather than the native USB port.

Note: These instructions are tested on Mac OSX and will vary for different
platforms.

    openocd -f ~/Library/Arduino15/packages/arduino/hardware/samd/1.6.6/variants/arduino_zero/openocd_scripts/arduino_zero.cfg

In another terminal from ``micropython/atmel-samd``:

    arm-none-eabi-gdb build-arduino_zero/firmware.elf
    (gdb) tar ext :3333
    ...
    (gdb) load
    ...
    (gdb) monitor reset init
    ...
    (gdb) continue

Connecting
----------

Serial
^^^^^^

All boards are currently configured to work over USB rather than UART. To
connect to it from OSX do something like this:

    screen /dev/tty.usbmodem142422 115200

You may not see a prompt immediately because it doesn't know you connected. To
get one either hit enter to get `>>>` or do CTRL-B to get the full header.

Mass storage
^^^^^^^^^^^^

All boards will also show up as a mass storage device. Make sure to eject it
before resetting or disconnecting the board.

Port Specific modules
---------------------

.. toctree::
    bindings/samd/__init__
