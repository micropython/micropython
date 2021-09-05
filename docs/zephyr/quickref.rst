.. _zephyr_quickref:

Quick reference for the Zephyr port
===================================

Below is a quick reference for the Zephyr port. If it is your first time working with this port please consider reading the following sections first:

.. toctree::
   :maxdepth: 1

   general.rst
   tutorial/index.rst

Running MicroPython
-------------------

See the corresponding section of the tutorial: :ref:`intro`.

Delay and timing
----------------

Use the :mod:`time <utime>` module::

    import time

    time.sleep(1)               # sleep for 1 second
    time.sleep_ms(500)          # sleep for 500 milliseconds
    time.sleep_us(10)           # sleep for 10 microseconds
    start = time.ticks_ms()     # get millisecond counter
    delta = time.ticks_diff(time.ticks_ms(), start) # compute time difference

Pins and GPIO
-------------

Use the :ref:`machine.Pin <machine.Pin>` class::

    from machine import Pin

    pin = Pin(("GPIO_1", 21), Pin.IN)   # create input pin on GPIO1
    print(pin)                          # print pin port and number

    pin.init(Pin.OUT, Pin.PULL_UP, value=1)     # reinitialize pin

    pin.value(1)                        # set pin to high
    pin.value(0)                        # set pin to low

    pin.on()                            # set pin to high
    pin.off()                           # set pin to low

    pin = Pin(("GPIO_1", 21), Pin.IN)   # create input pin on GPIO1

    pin = Pin(("GPIO_1", 21), Pin.OUT, value=1)         # set pin high on creation

    pin = Pin(("GPIO_1", 21), Pin.IN, Pin.PULL_UP)      # enable internal pull-up resistor

    switch = Pin(("GPIO_2", 6), Pin.IN)                 # create input pin for a switch
    switch.irq(lambda t: print("SW2 changed"))          # enable an interrupt when switch state is changed

Hardware I2C bus
----------------

Hardware I2C is accessed via the :ref:`machine.I2C <machine.I2C>` class::

    from machine import I2C

    i2c = I2C("I2C_0")          # construct an i2c bus
    print(i2c)                  # print device name

    i2c.scan()                  # scan the device for available I2C slaves

    i2c.readfrom(0x1D, 4)                # read 4 bytes from slave 0x1D
    i2c.readfrom_mem(0x1D, 0x0D, 1)      # read 1 byte from slave 0x1D at slave memory 0x0D

    i2c.writeto(0x1D, b'abcd')           # write to slave with address 0x1D
    i2c.writeto_mem(0x1D, 0x0D, b'ab')   # write to slave 0x1D at slave memory 0x0D

    buf = bytearray(8)                  # create buffer of size 8
    i2c.writeto(0x1D, b'abcd')          # write buf to slave 0x1D

Hardware SPI bus
----------------

Hardware SPI is accessed via the :ref:`machine.SPI <machine.SPI>` class::

    from machine import SPI

    spi = SPI("SPI_0")          # construct a spi bus with default configuration
    spi.init(baudrate=100000, polarity=0, phase=0, bits=8, firstbit=SPI.MSB) # set configuration

    # equivalently, construct spi bus and set configuration at the same time
    spi = SPI("SPI_0", baudrate=100000, polarity=0, phase=0, bits=8, firstbit=SPI.MSB)
    print(spi)                  # print device name and bus configuration

    spi.read(4)                 # read 4 bytes on MISO
    spi.read(4, write=0xF)      # read 4 bytes while writing 0xF on MOSI

    buf = bytearray(8)          # create a buffer of size 8
    spi.readinto(buf)           # read into the buffer (reads number of bytes equal to the buffer size)
    spi.readinto(buf, 0xF)      # read into the buffer while writing 0xF on MOSI

    spi.write(b'abcd')          # write 4 bytes on MOSI

    buf = bytearray(4)                  # create buffer of size 8
    spi.write_readinto(b'abcd', buf)    # write to MOSI and read from MISO into the buffer
    spi.write_readinto(buf, buf)        # write buf to MOSI and read back into the buf

Disk Access
-----------

Use the :ref:`zephyr.DiskAccess <zephyr.DiskAccess>` class to support filesystem::

    import os
    from zephyr import DiskAccess

    block_dev = DiskAccess('SDHC')      # create a block device object for an SD card
    os.VfsFat.mkfs(block_dev)           # create FAT filesystem object using the disk storage block
    os.mount(block_dev, '/sd')          # mount the filesystem at the SD card subdirectory

    # with the filesystem mounted, files can be manipulated as normal
    with open('/sd/hello.txt','w') as f:     # open a new file in the directory
        f.write('Hello world')                  # write to the file
    print(open('/sd/hello.txt').read())      # print contents of the file

Flash Area
----------

Use the :ref:`zephyr.FlashArea <zephyr.FlashArea>` class to support filesystem::

    import os
    from zephyr import FlashArea

    block_dev = FlashArea(4, 4096)      # creates a block device object in the frdm-k64f flash scratch partition
    os.VfsLfs2.mkfs(block_dev)          # create filesystem in lfs2 format using the flash block device
    os.mount(block_dev, '/flash')       # mount the filesystem at the flash subdirectory

    # with the filesystem mounted, files can be manipulated as normal
    with open('/flash/hello.txt','w') as f:     # open a new file in the directory
        f.write('Hello world')                  # write to the file
    print(open('/flash/hello.txt').read())      # print contents of the file

Sensor
------

Use the :ref:`zsensor.Sensor <zsensor.Sensor>` class to access sensor data::

    import zsensor
    from zsensor import Sensor

    accel = Sensor("FXOX8700")    # create sensor object for the accelerometer

    accel.measure()               # obtain a measurement reading from the accelerometer

    # each of these prints the value taken by measure()
    accel.float(zsensor.ACCEL_X)  # print measurement value for accelerometer X-axis sensor channel as float
    accel.millis(zsensor.ACCEL_Y) # print measurement value for accelerometer Y-axis sensor channel in millionths
    accel.micro(zsensor.ACCEL_Z)  # print measurement value for accelerometer Z-axis sensor channel in thousandths
    accel.int(zsensor.ACCEL_X)    # print measurement integer value only for accelerometer X-axis sensor channel
