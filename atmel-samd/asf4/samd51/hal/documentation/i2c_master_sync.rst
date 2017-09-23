=============================
I2C Master synchronous driver
=============================

I2C (Inter-Integrated Circuit) is a two wire serial interface usually used
for on-board low-speed bi-directional communication between controllers and
peripherals. The master device is responsible for initiating and controlling
all transfers on the I2C bus. Only one master device can be active on the I2C
bus at the time, but the master role can be transferred between devices on the
same I2C bus. I2C uses only two bidirectional open-drain lines, usually
designated SDA (Serial Data Line) and SCL (Serial Clock Line), with pull up
resistors.

The stop condition is automatically controlled by the driver if the I/O write and
read functions are used, but can be manually controlled by using the
i2c_m_sync_transfer function.

Often a master accesses different information in the slave by accessing
different registers in the slave. This is done by first sending a message to
the target slave containing the register address, followed by a repeated start
condition (no stop condition between) ending with transferring register data.
This scheme is supported by the i2c_m_sync_cmd_write and i2c_m_sync_cmd_read
function, but limited to 8-bit register addresses.

I2C Modes (standard mode/fastmode+/highspeed mode) can only be selected in
Atmel Start. If the SCL frequency (baudrate) has changed run-time, make sure to
stick within the SCL clock frequency range supported by the selected mode.
The requested SCL clock frequency is not validated by the
i2c_m_sync_set_baudrate function against the selected I2C mode.

Features
--------

	* I2C Master support
	* Initialization and de-initialization
	* Enabling and disabling
	* Run-time bus speed configuration
	* Write and read I2C messages
	* Slave register access functions (limited to 8-bit address)
	* Manual or automatic stop condition generation
	* 10- and 7- bit addressing
	* I2C Modes supported
	       +----------------------+-------------------+
	       |* Standard/Fast mode  | (SCL: 1 - 400kHz) |
	       +----------------------+-------------------+
	       |* Fastmode+           | (SCL: 1 - 1000kHz)|
	       +----------------------+-------------------+
	       |* Highspeed mode      | (SCL: 1 - 3400kHz)|
	       +----------------------+-------------------+

Applications
------------

* Transfer data to and from one or multiple I2C slaves like I2C connected sensors, data storage or other I2C capable peripherals
* Data communication between micro controllers
* Controlling displays

Dependencies
------------

* I2C Master capable hardware

Concurrency
-----------

N/A

Limitations
-----------

General
^^^^^^^

	* System Managmenet Bus (SMBus) not supported.
	* Power Management Bus (PMBus) not supported.

Clock considerations
^^^^^^^^^^^^^^^^^^^^

The register value for the requested I2C speed is calculated and placed in the correct register, but not validated if it works correctly with the clock/prescaler settings used for the module. To validate the I2C speed setting use the formula found in the configuration file for the module. Selectable speed is automatically limited within the speed range defined by the I2C mode selected.

Known issues and workarounds
----------------------------

N/A


