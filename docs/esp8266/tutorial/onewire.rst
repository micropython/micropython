Controlling 1-wire devices
==========================

The 1-wire bus is a serial bus that uses just a single wire for communication
(in addition to wires for ground and power).  The DS18B20 temperature sensor
is a very popular 1-wire device, and here we show how to use the onewire module
to read from such a device.

For the following code to work you need to have at least one DS18S20 or DS18B20 temperature
sensor with its data line connected to GPIO12.  You must also power the sensors
and connect a 4.7k Ohm resistor between the data pin and the power pin.  ::

    import time
    import machine
    import onewire, ds18x20

    # the device is on GPIO12
    dat = machine.Pin(12)

    # create the onewire object
    ds = ds18x20.DS18X20(onewire.OneWire(dat))

    # scan for devices on the bus
    roms = ds.scan()
    print('found devices:', roms)

    # loop 10 times and print all temperatures
    for i in range(10):
        print('temperatures:', end=' ')
        ds.convert_temp()
        time.sleep_ms(750)
        for rom in roms:
            print(ds.read_temp(rom), end=' ')
        print()

Note that you must execute the ``convert_temp()`` function to initiate a
temperature reading, then wait at least 750ms before reading the value.
