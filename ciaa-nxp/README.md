EDU-CIAA board support package
===============================

![CIAA logo](https://avatars0.githubusercontent.com/u/6998305?v=3&s=128)

[Spanish Version](README_ES.md)



[TOC]


## Requeriments
- gnu arm embedded (gcc+binutils+gdb)
- gnu make
- python3 with pyserial (`pip install pyserial` or `pip3 install pyserial `)
- openocd

## Usage mode
- Execute `make clean all download` to compile and download the program to EDU-CIAA board.
- Open a terminal on serial USB converter 2 (/dev/ttyUSB1 usualy)
- A python REPL interpreter is available via USB UART (DEBUG connector) into SERIAL2

## Execute software unit tests

Hardware connections for testing:

232_RX  ->  R100 ohm    ->  232_TX
CH1     ->  GNDA
CH2     ->  GNDA
CH3     ->  GNDA
GPIO8   ->  R100 ohm    ->  GPIO7
RS485   -> echo terminal (Use testing/rs485EchoTerminal)

- Execute:
`make clean`
`make test`
`make download`


##### Keybindings
###### Fancy REPL (aka `>>>` promt)
- CTRL+D restart interpreter (not board) aka soft reset
- CTRL+C cancel actual command
- CTRL+E enter paste mode (for paste code to editor)
- CTRL+A enter RAW REPL (for code sending)

###### RAW REPL (aka `>` promt)
- CTRL+B exit RAW REPL

###### PASTE MODE (aka `===` promt)
- CTRL+D finish and process paste mode
- CTRL+C cancel paste mode

## Hardware support module

### Four LEDS board supported via pyb.LED module.

Example:
```python
import pyb
led1 = pyb.LED(1)
led1.on()
pyb.delay(100);
led1.off()
```

Available led numbers: 1 to 6 (4:Red, 5:Green, 6:Blue)
> More info: http://test-ergun.readthedocs.org/en/latest/library/pyb.LED.html

### All board buttons via pyb.Switch.

Example:

```python
import pyb
switch1 = pyb.Switch(1)
val = switch1.switch()
print('sw1 vale:'+str(val))
```

```python
import pyb

def func(sw):
	print("sw pressed!")
	print(sw)

switch1 = pyb.Switch(1)
switch1.callback(func)
while True:
	pyb.delay(1000)
```



Available switch numbers:  1 to 4

> More info: http://test-ergun.readthedocs.org/en/latest/library/pyb.Switch.html

### UART support

RS232 UART is suported over P1 connector and RS485 UART over J1 via pyb.UART object.

Example:
```python
import pyb
uart = pyb.UART(3)
uart.init(115200,
          bits=8,
          parity=None,
          stop=1,
          timeout=1000,
          timeout_char=1000,
          read_buf_len=64)
uart.write("Hello World")
while True:
	if uart.any():
        print("hay data:")
        data = uart.readall()
        print(data)
```

Availabled UART are pyb.UART(0) (RS485) and pyb.UART(3) (RS232)

> More info: http://test-ergun.readthedocs.org/en/latest/library/pyb.UART.html

All interfaces of pyboard UART are preserver except the constructor on baudrate parameter.

Also, a packet reception mode was added. In this mode, the packet frame is stored in a internal buffer.

You check the buffer status with "any()" method and the method "readall()" return the complete frame.

To enable the packet reception, you need to add "packet_mode" argument as "True" on "init()" call.

Example:
```python
uart.init(115200,
          bits=8,
          parity=None,
          stop=1,
          timeout=0,
          timeout_char=1000,
          read_buf_len=64,
          packet_mode=True)
```

It is interpreted as a "package" when they begin to get bytes and the reception stops for longer time specified in the argument "timeout"

It is also possible to detect the end of the "package" with a particular character rather also timeout, for do it, use this character as argument value of "init".

Example:
```python
u0.init(115200,bits=8,
        parity=None,
        stop=1,
        timeout=0,
        timeout_char=1000,
        read_buf_len=64,
        packet_mode=True,
        packet_end_char=ord('o'))
```

In this example, The frame end is detected when a character 'o' is arrived.

## GPIO Support over pyb.Pin

Example:
```python
import pyb

p = pyb.Pin(0) #GPIO0
p.init(pyb.Pin.OUT_PP,pyb.Pin.PULL_NONE)
print(p)

while True:
        p.high()
        print("value:"+str(p.value()))
        pyb.delay(1000)
        p.low()
        print("value:"+str(p.value()))
        pyb.delay(1000)
```
Availabled GPIO is 0 to 8

> More info on: http://test-ergun.readthedocs.org/en/latest/library/pyb.Pin.html


## GPIO Interrupt support over pyb.ExtInt

Example:
```python
import pyb

def callBack(line):
        print("Pin Interrupt!")
        print("Line = ",line)

p = pyb.Pin(8)
p.init(pyb.Pin.OUT_PP,pyb.Pin.PULL_NONE)
print(p)

int = pyb.ExtInt(p,pyb.ExtInt.IRQ_RISING,pyb.Pin.PULL_NONE,callBack)
print(int)

while True:
        pyb.delay(1000)
        print("tick")
```
This implements four interrupts available to assign on any of 9 GPIO.

Methods:
  - enable()
  - disable()
  - swint()
  - line()

> More info on: http://test-ergun.readthedocs.org/en/latest/library/pyb.ExtInt.html


## DAC support over pyb.DAC 

Example:
```python
import pyb
import math

dac = pyb.DAC(1)

# sine
buf = bytearray(200) #100 samples. 2 bytes per sample
j=0
for i in range (0,len(buf)/2):
        v = 512 + int(511 * math.sin(2 * math.pi * i / (len(buf)/2) ) )
        buf[j+1] = (v >>  8) & 0xff
        buf[j] = v & 0xff
        j=j+2

# output the sine-wave at 400Hz
print("sine created")

dac.write_timed(buf, 400*(int(len(buf)/2)), mode=pyb.DAC.CIRCULAR)

while True:
        pyb.delay(1000)

```
There is only available DAC 1

A difference between this class and pyboard's class is that in this class 10bit resolution was implemented while pyboard's DAC class uses an 8bit value.
More info on: http://test-ergun.readthedocs.org/en/latest/library/pyb.DAC.html


## Timer support over pyb.Timer 

Example:
```python
import pyb

def callb(timer):
        print("Interval interrupt")
        print(timer)

def callbTimeout (timer):
        print("Timeout interrupt")
        print(timer)

print("Test Timers")

t1 = pyb.Timer(1)
t2 = pyb.Timer(2)
t1.interval(2000,callb)
t2.timeout(5000,callbTimeout)

while True:
        pyb.delay(1000)
```
Available timers: 0,1,2 and 3

Interval and timeout methods were added, these methods have two arguments: a time in miliseconds and the callback to be called.
More info on: http://test-ergun.readthedocs.org/en/latest/library/pyb.Timer.html
TimerChannel class was not implemented. Input capture and Output compare functionality is not present.

> Note: Board only have DAC 1

## PWM support over pyb.PWM

Example:
```python
import pyb

pyb.PWM.set_frequency(1000)

out0 = pyb.PWM(0)
out0.duty_cycle(50) # 50%
print("Duty cycle :"+str(out0.duty_cycle()))

out1= pyb.PWM(1)
out1.duty_cycle(25)

out10= pyb.PWM(10)
out10.duty_cycle(75)

while True:
        pyb.delay(1000)
```
Available PWM outs: 0 to 10

- 0: GPIO_2
- 1: GPIO_8
- 2: T_FIL1
- 3: T_FIL2
- 4: T_FIL3
- 5: T_COL0
- 6: T_COL1
- 7: T_COL2
- 8: LCD_1
- 9: LCD_2
- 10: LCD_3

The board has only 1 PWM module with 11 outs. There is only one frequency value and 11 duty cycle values (one per out). All PWM outs share the same frequency value.




## ADC support over pyb.ADC

Example:
```python
import pyb

channel1 = pyb.ADC(1)
channel2 = pyb.ADC(2)
channel3 = pyb.ADC(3)

while True:
        v1 = channel1.read()
        v2 = channel2.read()
        v3 = channel3.read()
        print("value ch1:"+str(v1))
        print("value ch2:"+str(v2))
        print("value ch3:"+str(v3))
        pyb.delay(1000)
```
AD Inputs available: 1,2 and 3. read() method returns the conversion's value (10 bit- 3.3V)


## Keyboard support over pyb.Keyboard (Poncho UI)

Example:
```python
import pyb

keyboard = pyb.Keyboard(4,4)

print(keyboard)

while True:
    key = keyboard.get_char()
    print("key:"+str(key))

```

Constructor's arguments are number of rows and number o columns. get_chat method will wait until a key is pressed. One byte is returned, most significative 4 bits represents number of row and less
significative 4 bits represents number of column. 

"get_matrix method" will not wait and it will return 0xFF if any key is pressed

## LCD HD44780U over pyb.LCD (Poncho UI)

Example:
```python
import pyb

lcd = pyb.LCD(2,0) # 2 lines, dot format:5x8

lcd.clear()

lcd.write("Test LCD\nEDUCIAA")
pyb.delay(1000)
lcd.clear()
lcd.config_cursor(True,True) #Cursor ON, Blink ON

c=0
while(True):
    c=c+1   
    lcd.goto_xy(0,0)
    lcd.write("counter:"+str(c))        
    lcd.goto_xy(10,1)
    lcd.write(str(c))
    pyb.delay(1000)

```

Constructor's arguments are number of lines (1,2,3 o 4) and dot format (0:5x8 - 1:5x10).


## Internal EEPROM over pyb.EEPROM

Primitive variables read and write example:
```python
import pyb

eeprom = pyb.EEPROM()

# R/W bytes test
eeprom.write_byte(0x0000,0x27)
eeprom.write_byte(0x0004,0x28)
for addr in range (0,16):
    val = eeprom.read_byte(addr)
    print(hex(val))

# R/W 32bit integers test
eeprom.write_int(0x0000,0x11223344)
eeprom.write_int(0x0004,0x12345678)
val = eeprom.read_int(0x0000)
print(hex(val))
val = eeprom.read_int(0x0004)
print(hex(val))


# R/W 64bit doubles test
eeprom.write_float(0x0000,3.14)
val = eeprom.read_float(0x0000)
print(str(val))
```
EEPROM class has methods for bytes, ints, floats and strings read and writing

- write_byte: Write a byte in the specified address and returns the number of bytes written (1 byte)
- write_int: Write an integer in the specified address and returns the number of bytes written (4 bytes)
- write_float: Write a float in the specified address and returns the number of bytes written (4 bytes)
- write : Write a String at the beginning of the memory (0x0000 address) 

- read_byte: Read a byte from the specified address
- read_int: Read an integer from the specified address
- read_float: Read a float from the specified address
- readall : Read a String from the beginning of the memory (0x0000 address)

EEPROM's size is 16Kbytes.

Serialize a python Dict using JSON example:
```python
import pyb
import json

dic = dict()
dic["k1"] = "Hello"
dic["k2"] = "World"
dic["k3"] = 2016
dic["k4"] = 3.14
print("Python Dict obj:")
print(dic)

#write dict in eeprom
jsonStr = json.dumps(dic)
print("JSON to write:")
print(jsonStr)
eeprom = pyb.EEPROM()
eeprom.write(jsonStr)


# read dict from eeprom
print("Python Dict obj from EEPROM:")
jsonStr = eeprom.readall()
dic = json.loads(jsonStr)
print(dic)
```

In this example a python Dict is created and serialized using json module. Then json String is written in EEPROM memory using "write" method.
Python Dict is rebuilt reading json string from EEPROM ("readall" method) and using "loads" method from json module


## SPI Master mode over pyb.SPI

Example (half-duplex):
```python
import pyb

spi = pyb.SPI(8,0,10000)

dataTx = bytearray()
dataTx.append(0x55)
dataTx.append(0x55)
dataTx.append(0x55)
dataTx.append(0x55)

while True:
    print("send:")
    print(dataTx)
    spi.write(dataTx)
    pyb.delay(1000)

    
    dataRx = spi.read(5)
    print("received:")
    print(dataRx)
    pyb.delay(1000)
```
Constructor receives 3 arguments:

- SPI frame's bits: 4 8 o 16
- SPI mode: 0,1,2 o 3
- Bitrate: expressed in Hz

Once spi object is created, it can be used for reading and writing data.
"write" method receives a bytearray and send it by the SPI bus.
"read" method receives the amount of frames to be read.


## RTC module over pyb.RTC

Example:
```python
import pyb
rtc = pyb.RTC()

# (year, month, day, weekday, hours, minutes, seconds)
#newDt = [2015,12,31,0,18,16,0]
#rtc.datetime(newDt)

while True:
    now = rtc.datetime()
    print(now)   
    pyb.delay(1000)
```

Method "datetime" Get or set the date and time of the RTC.
With no arguments, this method returns an 7-tuple with the current date and time. With 1 argument (being an 7-tuple) it sets the date and time.
The 7-tuple has the following format:
(year, month, day, weekday, hours, minutes, seconds)

Field "weekday" takes values from 0 to 6
RTC module will continue working after a CPU reset. If a battery is provided, RTC module will be working without main power supply.

Calibration Example:
```python
import pyb
rtc = pyb.RTC()
rtc.calibration(0)
newDt = [2015,12,31,0,18,16,0]
rtc.datetime(newDt)
```

Method "calibration" can periodically adjust the time counter either by not incrementing 
the counter, or by incrementing the counter by 2 instead of 1. This allows calibrating the 
RTC oscillator under some typical voltage and temperature conditions without the need to 
externally trim the RTC oscillator.

Values allowed are -131072 to 131072. Read http://www.nxp.com/documents/user_manual/UM10503.pdf for further information.
After calibration a datime must be loaded.


Back up registers example:
```python
import pyb
rtc = pyb.RTC()

#rtc.write_bkp_reg(0,27)
#rtc.write_bkp_reg(32,28)
#rtc.write_bkp_reg(63,29)

while True:
    print(rtc.read_bkp_reg(0))
    print(rtc.read_bkp_reg(32))
    print(rtc.read_bkp_reg(63))
    pyb.delay(1000)
```

There are 64 backup registers that will keep their values after a CPU reset or without main power supply if a battery is provided.

Method "write_bkp_reg" has two arguments : address (0 to 63) and value. This method will store the value in the specified address.
Method "read_bkp_reg" has one argument, address (0 to 63) and it will return the backup register's value stored with "write_bkp_reg".


Alarm example:
```python
import pyb
rtc = pyb.RTC()

newDt = [2015,12,31,0,20,15,0]
rtc.datetime(newDt)

def rtcCallback(rtc):
    print("Alarm int!")

alarmDt = [2015,12,31,0,20,16,10]
rtc.alarm_datetime(alarmDt,pyb.RTC.MASK_SEC | pyb.RTC.MASK_MIN)
rtc.callback(rtcCallback)

print("alarm:")
print(rtc.alarm_datetime())
```

In this example "rtcCallback" function is invoked when alarm datetime is equals to current datetime. 
"alarm_datetime" method configures or returns an 7-tuple with the configurated date and time for RTC alarm. This method has a second argument 
"alarm mask" that is built with the followings constants:

- pyb.RTC.MASK_SEC : Seconds field is used for comparation.
- pyb.RTC.MASK_MIN : Minutes field is used for comparation.
- pyb.RTC.MASK_HR : Hours field is used for comparation.
- pyb.RTC.MASK_DAY : Day field is used for comparation.
- pyb.RTC.MASK_MON : Month field is used for comparation.
- pyb.RTC.MASK_YR : Year field is used for comparation.
- pyb.RTC.MASK_DOW : Day of week field is used for comparation.

Using "callback" method a callback function can be set and it will execute at alarm time. In this example the mask is built using seconds and minutes
so alarm interrupt will happen each hour at 16 minutes and 10 seconds.

Alarm can be disable using "alarm_disable" method.



## Time support module

### Time operations supported via utime module.

Example:
```python
import pyb
import utime

print("utime Test")

rtc = pyb.RTC()

while True:
    now = rtc.datetime()
    print(now)
    
    ts = utime.time()
    print("timestamp:"+str(ts))
    dateTimeTuple = utime.localtime(ts)
    print(dateTimeTuple)
    ts = utime.mktime(dateTimeTuple)
    print("mktime timestamp:"+str(ts))
    print("__________________________")

    utime.sleep(1)
```

- localtime([secs]): Convert a time expressed in seconds since Jan 1, 2000 into an 8-tuple which contains: (year, month, mday, hour, minute, second, weekday, yearday) If secs is not provided or None, then the current time from the RTC is used. year includes the century (for example 2014) month is 1-12 mday is 1-31 hour is 0-23 minute is 0-59 second is 0-59 weekday is 0-6 for Mon-Sun. yearday is 1-366
- mktime(): This is inverse function of localtime. It's argument is a full 8-tuple which expresses a time as per localtime. It returns an integer which is the number of seconds since Jan 1, 2000.
- sleep(seconds): Sleep for the given number of seconds. Seconds can be a floating-point number to sleep for a fractional number of seconds.
- time(): Returns the number of seconds, as an integer, since 1/1/2000.

NOTE: RTC module must be initialized before utime utilization

> More info: https://micropython.org/doc/module/time


## I2C support module

### I2C master mode via pyb.I2C module

24C04 eeprom memory with I2C interface example:
```python
import pyb

print("Test I2C with AT24C04")

i2c = pyb.I2C(100000) #100Khz
i2c.slave_addr(0x50)

def writeByte(addr,value):
    data = bytearray()
    data.append(addr)
    data.append(value)
    i2c.write(data)


def readBytes(addr,size):
    data = bytearray()
    data.append(addr)    
    i2c.write(data)    
    return i2c.read(size)

writeByte(0x00,11)
pyb.delay(10)
writeByte(0x01,22)
pyb.delay(10)
writeByte(0x02,33)
pyb.delay(10)


data = readBytes(0x00,16)
print("[0]:"+str(data[0]))
print("[1]:"+str(data[1]))
print("[2]:"+str(data[2]))
```
I2C constructor receive as argument the bus speed (100Khz or 400Khz) expressed in Hz.
slave_addr method configure slave's address used each time bytes are written or read.
Once spi object is created, it can be used for reading and writing data.
"write" method receives a bytearray and send it by the I2C bus.
"read" method receives the amount of bytes to be read.

In this example two functions are written, the first one allows to write a byte in a specified address in the memory and the second one reads a specified amont of
bytes from the specified address.


## Modbus module

### Modbus master support with Instrument class.

Example:Reading register 0x0001 from slave 0x55 using uart 3

```python
import pyb
import ModBus

uart = pyb.UART(3)
uart.init(115200)

instrument = ModBus.Instrument(uart,0x55,mode=ModBus.MODE_ASCII)
print(instrument)

while True:
    try:
        v = instrument.read_register(1)
        print("Register 1 value :"+str(v))
    except Exception as e:
        print(e)
    pyb.delay(1000)
```
Instrument class needs an uart object that represents a serial interface in the board, the slave address -0x55 in this example- and the modbus mode
constans allowed are:
- ModBus.MODE_ASCII
- ModBus.MODE_RTU

instrument object can be used to read and write registers through the serial interface in the defined slave using modbus protocol. 
Supported methods are:

- read_bit(self, registeraddress, functioncode=2)
Read one bit from the slave.

        Args:
            * registeraddress (int): The slave register address (use decimal numbers, not hex).
            * functioncode (int): Modbus function code. Can be 1 or 2.

        Returns:
            The bit value 0 or 1 (int).

        Raises:
            ValueError, TypeError, IOError


- write_bit(self, registeraddress, value, functioncode=5)
Write one bit to the slave.

        Args:
            * registeraddress (int): The slave register address (use decimal numbers, not hex).
            * value (int): 0 or 1
            * functioncode (int): Modbus function code. Can be 5 or 15.

        Returns:
            None

        Raises:
            ValueError, TypeError, IOError


- read_register(self, registeraddress, numberOfDecimals=0, functioncode=3, signed=False)
Read an integer from one 16-bit register in the slave, possibly scaling it.

        The slave register can hold integer values in the range 0 to 65535 ("Unsigned INT16").

        Args:
            * registeraddress (int): The slave register address (use decimal numbers, not hex).
            * numberOfDecimals (int): The number of decimals for content conversion.
            * functioncode (int): Modbus function code. Can be 3 or 4.
            * signed (bool): Whether the data should be interpreted as unsigned or signed.

        If a value of 77.0 is stored internally in the slave register as 770, then use ``numberOfDecimals=1``
        which will divide the received data by 10 before returning the value.

        Similarly ``numberOfDecimals=2`` will divide the received data by 100 before returning the value.

        Returns:
            The register data in numerical value (int or float).

        Raises:
            ValueError, TypeError, IOError


- write_register(self, registeraddress, value, numberOfDecimals=0, functioncode=16, signed=False)
Write an integer to one 16-bit register in the slave, possibly scaling it.

        The slave register can hold integer values in the range 0 to 65535 ("Unsigned INT16").

        Args:
            * registeraddress (int): The slave register address  (use decimal numbers, not hex).
            * value (int or float): The value to store in the slave register (might be scaled before sending).
            * numberOfDecimals (int): The number of decimals for content conversion.
            * functioncode (int): Modbus function code. Can be 6 or 16.
            * signed (bool): Whether the data should be interpreted as unsigned or signed.

        To store for example ``value=77.0``, use ``numberOfDecimals=1`` if the slave register will hold it as 770 internally.
        This will multiply ``value`` by 10 before sending it to the slave register.

        Similarly ``numberOfDecimals=2`` will multiply ``value`` by 100 before sending it to the slave register.

        Returns:
            None

        Raises:
            ValueError, TypeError, IOError


- read_registers(self, registeraddress, numberOfRegisters, functioncode=3)
Read integers from 16-bit registers in the slave.

        The slave registers can hold integer values in the range 0 to 65535 ("Unsigned INT16").

        Args:
            * registeraddress (int): The slave register start address (use decimal numbers, not hex).
            * numberOfRegisters (int): The number of registers to read.
            * functioncode (int): Modbus function code. Can be 3 or 4.

        Any scaling of the register data, or converting it to negative number (two's complement)
        must be done manually.

        Returns:
            The register data (a list of int).

        Raises:
            ValueError, TypeError, IOError


- write_registers(self, registeraddress, values)
Write integers to 16-bit registers in the slave.

        The slave register can hold integer values in the range 0 to 65535 ("Unsigned INT16").

        Uses Modbus function code 16.

        The number of registers that will be written is defined by the length of the ``values`` list.

        Args:
            * registeraddress (int): The slave register start address (use decimal numbers, not hex).
            * values (list of int): The values to store in the slave registers.

        Any scaling of the register data, or converting it to negative number (two's complement)
        must be done manually.

        Returns:
            None

        Raises:
            ValueError, TypeError, IOError



### Modbus slave support with Slave class.

Example:

```python
import pyb
import ModBus

uart = pyb.UART(3)
uart.init(115200)
led1 = pyb.LED(1)

mappedRegs = {0x4000 : 0x0000 , 0x4001 : 0x0000}

modbus = ModBus.Slave(uart,0x55,mappedRegs,mode=ModBus.MODE_ASCII)
print(modbus)

print(mappedRegs)

while True:
    if modbus.receive():
        print(mappedRegs)
        if mappedRegs[0x4000]==0:
            led1.off()
        else:
            led1.on() 
```

In slave mode, a dictionary is required. This object will represent slave's registers. Dictionary keys are register addresses, in this example
the defined slave has two registers in address 0x4000 and 0x4001 with 0x0000 values.
Read and write registers modbus packets are supported, write commands will modify dictionary values, and read commands will read them.

Slave object is created passing a uart object, slave address (0x55 in this example), the dictionary and modbus mode (ascii or rtu).
"receive" method must be call constantly to receive modbus packets through uart object. This method will return True if a packet was received.





