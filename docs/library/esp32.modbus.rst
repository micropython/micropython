.. currentmodule:: modbus

:mod:`modbus` --- modbus library for the ESP32
==============================================

.. module:: modbus
    :synopsis: modbus library for the ESP32

The ``modbus`` module contains functions and classes specifically aimed at
running a modbus master or modbus slave on the ESP32.

Modbus master
-------------

The modbus master is the one, that initiates the communication and reads the 
registers on the modbus slave. There are two types of modbus masters implemented: 

    * The ``modbus.Serial_Master`` talks over the serial interface (typically via
      the RS485 protocol). Here you can choose between two modes: 
      ``modbus.SERIAL_MODE_ASCII`` and ``modbus.SERIAL_MODE_RTU``, which is the 
      default.

    * The ``modbus.TCP_Master`` is a TCP client.

The master reads or writes data to different types of registers, which are typically
explained in the device manual. There are 4 different types of registers:

    * ``modbus.HOLDING`` - can be read and written. Contain data like device 
      configuration, software version, or output data, like DAC, PWM, ...
    
    * ``modbus.COIL`` - can be read and written. Contain discrete output data, like
      states of LEDs or Relais.

    * ``modbus.INPUT`` - can only be read. Contain analog input data, like measured
      temperatures or voltages.

    * ``modbus.DISCRETE`` - can only be read. Contain discrete input data, like the 
      state of a switch or a alarm state.

Of cause, there is nothing holding you back from storing analog input data in a holding 
register. You just have to think about what will happen, if someone tries to write to 
this register.

The registers are defined with the ``modbus.Parameter_Descriptor``, but only on the 
modbus master side. On the slave side you have to use the ``modbus.Register_Area``, but 
more on that later.

Here is a short example of a modbus RTU master, that reads a power meter::

    import modbus

    master = modbus.Serial_Master(
        2,                  # UART port
        9600,               # baudrate
        modbus.PARITY_EVEN  # parity
        )
    
    voltage = modbus.Parameter_Descriptor(
        "Voltage",          # Name, must be unique
        "V",                # Unit, just for documentation, can be an empty string
        1,                  # Slave ID
        modbus.HOLDING,     # Register area
        305,                # Register
        1,                  # Number of registers to read
        modbus.TYPE_U16     # Type of the result
        )

    master.run()

    voltage.read()          # returns 23378, which is the actual voltage * 100
                            # see device manual

.. Warning::
    First you have to create the master, then declare the parameter descriptors
    and then start the master. It is not possible to add parameter descriptors after 
    you started the master. Also you can't start the master, if you haven't declared 
    at least one parameter descriptor.

.. class:: modbus.Serial_Master(uart_port, baudrate, parity, tx, rx, rts, serial_mode)

    The modbus serial master. Reads/Writes data to a modbus serial slave like a power 
    meter or another esp32.

        * *uart_port*: the used UART port. They each have default GPIO assigned to them, 
          however depending on your ESP32 variant and board, these pins may conflict with 
          embedded flash, onboard PSRAM or peripherals.

        * *baudrate*: default 9600

        * *parity*: default modbus.PARITY_NONE

        * *tx*: default 12, the used tx pin

        * *rx*: default 13, the used rx pin
        
        * *rts*: default 32, the used rts pin

        * *serial_mode*: default modbus.SERIAL_MODE_RTU, could also be SERIAL_MODE_ASCII


.. class:: modbus.TCP_Master(list_of_IPs, port, network_interface)

    The modbus TCP client. Reads/Writes data to a modbus TCP server.

        * *list_if_IPs*: list of IP addresses. You can read/write to multiple servers. Refer
          to the first server with the slave id 1 in the parameter descriptor and so on.

        * *port*: default 502, the used TCP port

        * *network_interface*: see example. Currently only possible with the ``network2`` 
          module::

            import network2
            import modbus
            import time

            w = network2.Wifi()
            w.active(True)
            w.connect("SSID", "Password")

            while not w.connected:
                time.sleep(1)

            master = modbus.TCP_Master(["192.168.178.76"], 502, w)
            voltage = modbus.Parameter_Descriptor( ... )
            
            master.run()
            voltage.read()


.. method:: master.run()

    Starting the underlying modbus stack. You have to define the parameter descriptors first!


.. class:: modbus.Parameter_Descriptor(key, unit, slave_id, area, register, size, data_type)

    The parameter descriptor is used read or write data on the slave.

        * *key*: name of the parameter, must be unique.

        * *unit*: the unit, like degC, V or m. Can be an empty string.

        * *slave_id*: integer. For modbus RTU devices: see manual. For TCP devices: the index of 
          the IP in the ``TCP_Master`` consturctor.

        * *area*: the register area: ``modbus.HOLDING``, ``modbus.COIL``, ``modbus.INPUT`` or 
          ``modbus.DISCRETE``.

        * *register*: the starting register of the parameter.

        * *size*: number of registers to read/write. One register is two bytes long, so if you
          set the size to 10, you could store 19 ASCII chars (+terminator) in this register

        * *data_type*: ``modbus.TYPE_U8``, ``modbus.TYPE_U16``, ``modbus.TYPE_U32``, ``modbus.TYPE_FLOAT``
          or ``modbus.TYPE_ASCII``.


.. method:: parameter.read()

    Read the parameter from slave device. Master must be running!

    Return value depends on the register area and the data type:

        * Holding, Input: return value is an integer, float or string.

        * Coil, Discrete: return value is a list of boolean.

.. method:: parameter.write(data)

    Write the parameter on the slave device. Master must be running!

        * *data*: type depends on the register area and the data type:

            * Holding, Input: integer, float or string.

            * Coil, Discrete: integer or list of boolean.

Here is a short example of a TCP client that reads from multiple TCP servers::

    import network2
    import modbus
    import time

    w = network2.Wifi()
    w.active(True)
    w.connect("SSID", "Password")

    while not w.connected:
        time.sleep(1)

    master = modbus.TCP_Master(["192.168.178.76", "192.168.178.77"], 502, w)
    
    # version of server 1 (192.168.178.76)
    versionS1 = modbus.Parameter_Descriptor("Version_Server1", "", 1, # <- Note: slave ID = 1
        modbus.INPUT, 100, 10, modbus.TYPE_ASCII)

    # version of server 2 (192.168.178.77)
    versionS2 = modbus.Parameter_Descriptor("Version_Server2", "", 2, # <- Note: slave ID = 2
        modbus.INPUT, 100, 10, modbus.TYPE_ASCII)

    # temperature sensor connected to server 1
    temperatureS1 = modbus.Parameter_Descriptor("Temperature_Sensor1", "degC", 1, 
        modbus.INPUT, 110, 2, modbus.TYPE_FLOAT)
    
    # temperature sensor connected to server 1
    temperatureS2 = modbus.Parameter_Descriptor("Temperature_Sensor2", "degC", 2, 
        modbus.INPUT, 110, 2, modbus.TYPE_FLOAT)

    # alarm temperature for sensor temperatureS1
    alarmS1 = modbus.Parameter_Descriptor("Alarm_Sensor1", "degC", 1, 
        modbus.HOLDING, 100, 2, modbus.TYPE_FLOAT)
    
    # 4 leds connected to server 2
    ledsS2 = modbus.Parameter_Descriptor("LEDs_Server2", "", 2, 
        modbus.COIL, 100, 4)

    master.run()

    versionS1.read()
    # Output: "Modbus_TCP_Srv_V0.1"

    versionS2.read()
    # Output: "Modbus_TCP_Srv_V0.2"
    
    temperatureS1.read()
    # Output: 15.43
    
    temperatureS2.read()
    # Output: 23.65

    alarmS1.read()
    # Output: 30.00

    alarmS1.write(25.00)

    alarmS1.read()
    # Output: 25.00

    ledsS2.write(0x05)
    # Output: [True, False, True, False]


Modbus slave
------------

The modbus slave provides data to master. The data can be read and written. The slave
can't initiate the communication, it must always be done by the master. As for the 
master, there are two types of slaves:

    * The ``modbus.Serial_Slave`` is the counter part to the ``modbus.Serial_Master``.
      You can choose between two modes: ``modbus.SERIAL_MODE_ASCII`` and 
      ``modbus.SERIAL_MODE_RTU``, which is the default.

    * The ``modbus.TCP_Slave`` is a TCP server.

You have to define the registers with the ``modbus.Register_Area``. There can be multiple
areas per type, but they have to be located at different addresses::

    import modbus

    slave = modbus.Serial_Slave(
        1,                  # slave id
        2,                  # UART port
        9600,               # baudrate
        modbus.PARITY_EVEN  # parity
    )

    a_HOLDING_1 = modbus.Register_Area(
        modbus.HOLDING,     # type
        100,                # start register
        100                 # length, one register = 2 bytes
    )
    
    a_HOLDING_2 = modbus.Register_Area(modbus.HOLDING, 300, 100)
    a_DISCRETE = modbus.Register_Area(modbus.DISCRETE, 1, 100)
    a_COIL = modbus.Register_Area(modbus.COIL, 1, 100)
    a_INPUT = modbus.Register_Area(modbus.INPUT, 1, 100)

    slave.run()

.. Warning::
    Like for the master, you have to create the slave first, then define the register areas 
    and then start the slave. Defining register areas after you start the slave is not allowed.

.. class:: modbus.Serial_Slave(slave_id, art_port, baudrate, parity, tx, rx, rts, serial_mode, callback)

    The modbus serial slave. Provides registers, that can be read or written by the master.

        * *slave_id*: the modbus slave id

        * *uart_port*: the used UART port. They each have default GPIO assigned to them, 
          however depending on your ESP32 variant and board, these pins may conflict with 
          embedded flash, onboard PSRAM or peripherals.

        * *baudrate*: default 9600

        * *parity*: default modbus.PARITY_NONE

        * *tx*: default 12, the used tx pin

        * *rx*: default 13, the used rx pin
        
        * *rts*: default 32, the used rts pin

        * *serial_mode*: default modbus.SERIAL_MODE_RTU, could also be SERIAL_MODE_ASCII

        * *callback*: method, that is called, whenever a register is read or written. Only parameter of the callback
          us a tuple ``(event, register, size)``. ``event`` is one of ``modbus.EVENT_...``::

            import modbus

            def cb(param):
                if param[0] == modbus.EVENT_HOLDING_REG_RD:  print("HOLDING READ  - Reg: {}, Size: {}".format(param[1], param[2]))
                if param[0] == modbus.EVENT_HOLDING_REG_WR:  print("HOLDING WRITE - Reg: {}, Size: {}".format(param[1], param[2]))
                if param[0] == modbus.EVENT_INPUT_REG_RD:    print("INPUT READ    - Reg: {}, Size: {}".format(param[1], param[2]))
                if param[0] == modbus.EVENT_COILS_REG_RD:    print("COILS READ    - Reg: {}, Size: {}".format(param[1], param[2]))
                if param[0] == modbus.EVENT_COILS_REG_WR:    print("COILS WRITE   - Reg: {}, Size: {}".format(param[1], param[2]))
                if param[0] == modbus.EVENT_DISCRETE_REG_RD: print("DISCRETE READ - Reg: {}, Size: {}".format(param[1], param[2]))

            slave = modbus.Serial_Slave(1, 2, 9600, modbus.PARITY_EVEN, callback=cb)
            area_1 = modbus.Register_Area( ... )
            
            slave.run()


.. class:: modbus.TCP_Slave(port, network_interface, callback)

    The modbus TCP server. Provides registers, that can be read or written by the TCP client.

        * *port*: default 502, the used TCP port

        * *network_interface*: see example. Currently only possible with the ``network2`` 
          module::

            import network2
            import modbus
            import time

            w = network2.Wifi()
            w.active(True)
            w.connect("SSID", "Password")

            while not w.connected:
                time.sleep(1)

            slave = modbus.TCP_Slave(502, w)
            area_1 = modbus.Register_Area( ... )
            
            slave.run()
        
        * *callback*: method, that is called, whenever a register is read or written. Only parameter of the callback
          us a tuple ``(event, register, size)``. ``event`` is one of ``modbus.EVENT_...``. See ``Serial_Slave`` example.


.. method:: slave.run()

    Starting the underlying modbus stack. You have to define the register areas first!


.. class:: modbus.Register_Area(area, register, size)

    Register_Area stores the data that is read/written by the master.

        * *area*: the register area: ``modbus.HOLDING``, ``modbus.COIL``, ``modbus.INPUT`` or 
          ``modbus.DISCRETE``.

        * *register*: the starting register of the parameter.

        * *size*: number of registers to read/write. One register is two bytes long, so if you
          set the size to 10, you could store 19 ASCII chars (+terminator) in this register or 
          up to 160 bits.

.. method:: area.read(register, data_type, [length])

    Read the data and interpret it as data_type.

        * *register*: the start register.

        * *data_type*: ``modbus.TYPE_U8``, ``modbus.TYPE_U16``, ``modbus.TYPE_U32``, ``modbus.TYPE_FLOAT``
          or ``modbus.TYPE_ASCII``. If *data_type* is ASCII, you have to provide a length.

        * *length*: number of registers to read. Only for ``modbus.TYPE_ASCII``.

.. method:: area.write(register, data_type, data)

    Write the data to a register (or more than one).

        * *register*: the start register.

        * *data_type*: ``modbus.TYPE_U8``, ``modbus.TYPE_U16``, ``modbus.TYPE_U32``, ``modbus.TYPE_FLOAT``
          or ``modbus.TYPE_ASCII``.

        * *data*: the data to be written. Make sure, it fits to the date type. Also, if you write a string
          that is too long, the following registers will be overwritten.

.. method:: area.read_raw(register, length)

    Reads the data and returns a ``bytearray``.

        * *register*: the start register.

        * *length*: number of registers to read. 

.. method:: area.write_raw(register, length, data)

    Write the data to the area.

        * *register*: the start register.

        * *length*: number of registers to write.

        * *data*: data, that is written. Must be a ``bytearray`` and at least two times as long as *length*.

.. method:: area.read_registers(register, length)

    Reads the data and returns a list of integers.

        * *register*: the start register.

        * *length*: number of registers to read. 

.. method:: area.write_registers(register, data)

    Write the data to the area.

        * *register*: the start register

        * *data*: data, that is written. Must be a list of integers and at least as long as *length*.

.. method:: area.read_bits(start_bit, length)

    Reads the data and returns a list of boolean.

        * *start_bit*: the first bit to read.

        * *length*: number of bits to read. 

.. method:: area.write_bits(start_bit, data)

    Write the bits to the area.

        * *register*: the first bit to write.

        * *data*: data, that is written. Must be a list of boolean and at least as long as *length*.
    