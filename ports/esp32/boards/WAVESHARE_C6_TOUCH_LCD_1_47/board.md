The Waveshare ESP32-C6-Touch-LCD-1.47 uses an ESP32-C6FH8 with 8MiB of flash.
It includes a 172x320 JD9853 SPI LCD, an AXS5106L capacitive touch controller,
a QMI8658A IMU, and a microSD card slot.

The default I2C bus is shared by the touch controller and IMU::

    I2C(0, scl=Pin(19), sda=Pin(18))

The default SPI bus is shared by the LCD and microSD card::

    SPI(1, sck=Pin(1), mosi=Pin(2), miso=Pin(3))

Use the board pin aliases ``LCD_*``, ``TP_*``, ``IMU_INT*``, and ``SD_*`` for
the onboard peripherals.  The LCD and microSD card share SCK and MOSI, so keep
the chip select of the unused peripheral inactive when using either device.
