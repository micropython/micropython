EDU-CIAA board support package
===============================

![CIAA logo](https://avatars0.githubusercontent.com/u/6998305?v=3&s=128)

[Spanish Version](README_ES.md)


[TOC]


## Usage mode
- Open py/Main.py and write the Python script
- Execute `make download` to compile and download the program to EDU-CIAA board. This embed the py/Main.py code into firmware.
- Optionaly, a python REPL interpreter is available via USB UART (DEBUG connector) into SERIAL2

## Included files
- py/ -- Main.py place. This file contain the python code execute at startup.
- py2c.py -- Python Scripy to embed the py/Main.py into firmware (via C array)

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
More info: http://test-ergun.readthedocs.org/en/latest/library/pyb.LED.html

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
More info: http://test-ergun.readthedocs.org/en/latest/library/pyb.Switch.html

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

More info: http://test-ergun.readthedocs.org/en/latest/library/pyb.UART.html

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

More info on: http://test-ergun.readthedocs.org/en/latest/library/pyb.Pin.html


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

More info on: http://test-ergun.readthedocs.org/en/latest/library/pyb.ExtInt.html


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

Ejemplo:
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

