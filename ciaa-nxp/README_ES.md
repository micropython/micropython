Soporte para EDU-CIAA
========================
Table of context

[TOC]

<p align="center">
  <img src="https://avatars0.githubusercontent.com/u/6998305?v=3&s=128" alt="CIAA Logo"/>
</p>

### Modo de uso:

- Ejecutar `make clean all download` para compilar y bajar el programa en la EDU-CIAA.
- Ejecute una terminal serial a 115200 en el segundo puerton serial del USB (usualmente /dev/ttyUSB1 en Linux)
- Puede usar una linea de comandos python REPL en la UART del puerto USB de DEBUG.


## Ejecutar software unit tests

Conexiones de Hardware para testing:

232_RX  ->  R100 ohm    ->  232_TX
CH1     ->  GNDA
CH2     ->  GNDA
CH3     ->  GNDA
GPIO8   ->  R100 ohm    ->  GPIO7
RS485   -> echo terminal (Usar testing/rs485EchoTerminal)

- Ejecutar: 
`make clean`
`make test`
`make download`


## Soporte de hardware

### Modulo pyb:
##### Soporte de los 4 LEDS

Clase `pyb.LED`.

Ejemplo:
```python
import pyb
led1 = pyb.LED(1)
led1.on()
pyb.delay(100);
led1.off()
```
Numeros de leds disponibles: de 1 a 6 (4:Red, 5:Green, 6:Blue)
Mas info en : http://test-ergun.readthedocs.org/en/latest/library/pyb.LED.html

##### Soporte para los 4 pulsadores de la placa
Clase `pyb.Switch`.

Ejemplo:
```python
import pyb
switch1 = pyb.Switch(1)
val = switch1.value()
print('sw1 vale:'+str(val))
```
Numeros de switch disponibles:  de 1 a 4
Mas info en : http://test-ergun.readthedocs.org/en/latest/library/pyb.Switch.html

##### Soporte para la UART

Implementado RS232 del conector P1 y la UART RS485 de la placa mediante la clase `pyb.UART`.

Ejemplo:

```python
import pyb
uart = pyb.UART(3)
uart.init(115200,bits=8, parity=None, stop=1,timeout=1000, timeout_char=1000, read_buf_len=64)
uart.write("Hola mundo")
while True:
        if uart.any():
                print("hay data:")
                data = uart.readall()
                print(data)
```
Las UARTs disponibles son la "0" (RS485) y la "3" (RS232)

Mas info en : http://test-ergun.readthedocs.org/en/latest/library/pyb.UART.html
Se respeto la compatibilidad del modulo original de la pyboard exceptuando el constructor que recibe el baudrate.
Tambien se agrego un modo de recepcion por "paquete" en donde la trama que llega se almacena en un buffer interno y luego el metodo "any()" devuelve True.
Utilizando el metodo "readall()" se obtiene la trama completa. Para habilitar la recepcion por paquete se deben agregar el argumento "packet_mode" en True al final del metodo "init()". Ejemplo:
```python
uart.init(115200,bits=8, parity=None, stop=1,timeout=0, timeout_char=1000, read_buf_len=64,packet_mode=True)
```
Se interpretara como un "paquete" cuando comiencen a llegar bytes y se detenga la recepcion durante mas tiempo del tiempo especificado en el argumento "timeout"

Tambien es posible detectar el final del "paquete" mediante un caracter en particular en lugar de hacerlo por timeout, para ello se puede agregar dicho caracter como argumento en "init". Ejemplo:
```python
art.init(115200,bits=8, parity=None, stop=1,timeout=0, timeout_char=1000, read_buf_len=64,packet_mode=True,packet_end_char=ord('o'))
```
En este ejemplo, se detectara un fin de trama cuando llegue el caracter 'o'


##### Soporte para GPIOs

Clase `pyb.Pin`.

Ejemplo:
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
Las GPIOs disponibles van de 0 a 8

Mas info en : http://test-ergun.readthedocs.org/en/latest/library/pyb.Pin.html


##### Soporte para interrupciones en GPIOs
Clase `pyb.ExtInt`.

Ejemplo:
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
Existen 4 interrupciones disponibles para asignar a cualquiera de las 9 GPIOs
Se implementaron los metodos:
- enable()
- disable()
- swint()
- line()

Mas info en : http://test-ergun.readthedocs.org/en/latest/library/pyb.ExtInt.html

##### Soporte para DAC
Clase `pyb.DAC`.

Ejemplo:
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
Existe solo el DAC 1

A diferencia de la clase DAC de la pyboard (http://test-ergun.readthedocs.org/en/latest/library/pyb.DAC.html) se utilizaron valores de 10bit en vez de 8bits para aprovechar al maximo la resolucion del DAC.


##### Soporte para Timers

Clase `pyb.Timer`.

Ejemplo:
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
Los timers disponibles son el 0,1,2 y 3

Ademas de las funciones definidas en http://test-ergun.readthedocs.org/en/latest/library/pyb.Timer.html se agregaron los metodos interval y timeout. Ambos reciben
el tiempo en milisegundos y una funcion callback. El primero ejecutara la funcion cada el tiempo prefijado, mientras que el segundo la ejecutara solo una vez luego
del tiempo prefijado.

No se implemento la clase TimerChannel, por lo que las funcionalidades de Output Compare e Input Capture no son accesibles.


##### Soporte para PWM

Clase `pyb.PWM`

Ejemplo:
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
Salidas de PWM disponibles: 0 a 10

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

La placa posee un solo modulo PWM con 11 salidas asociadas, por esta razon todas las salidas comparten la misma frecuencia, pero tienen un valor de ciclo de actividad independiente.


##### Soporte para ADC

Clase `pyb.ADC`.

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
Entradas AD disponibles: 1,2 y 3. El resultado del metodo read es el valor de conversion (10 bit de resolucion en 3.3V)


##### Soporte para Keyboard (Poncho UI)

Clase `pyb.Keyboard`.

Ejemplo:
```python
import pyb

keyboard = pyb.Keyboard(4,4)

print(keyboard)

while True:
    key = keyboard.get_char()
    print("key:"+str(key))

```

El constructor recibe la cantidad de filas y columnas que se sensan. El metodo get_char se quedara esperando que se presione una tecla, se devolvera un byte en donde los 4 bits de mas peso corresponden
con el numero de fila y los 4 bits de menor peso corresponden con el numero de columna. Tambien puede utilizarse el metodo "get_matrix" el cual no es bloqueante y devolvera 0xFF si ninguna tecla es presionada.


##### Soporte para LCD HD44780U (Poncho UI)

Clase `pyb.LCD`.

Ejemplo:
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

El constructor recibe la cantidad de lineas (1,2,3 o 4) y el formato del caracter (0:5x8 - 1:5x10). 


  

##### Soporte para EEPROM interna

Clase `pyb.EEPROM`.

Ejemplo lectura y escritura de variables primitivas:
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
La clase EEPROM posee metodos para leer y escribir bytes (8bit) enteros (32bits), numeros con punto flotante (32bits) y Strings.

- write_byte: Escribe un byte en la direccion indicada y devuelve la cantidad de bytes escritos (1 byte)
- write_int: Escribe un entero en la direccion indicada y devuelve la cantidad de bytes escritos (4 bytes)
- write_float: Escribe un float en la direccion indicada y devuelve la cantidad de bytes escritos (4 bytes)
- write : Escribe un String a partir de la direccion 0x0000 de la EEPROM

- read_byte: Lee un byte en la direccion indicada
- read_int: Lee un entero desde la direccion indicada
- read_float: Lee un float desde la direccion indicada
- readall : Lee un string desde la direccion 0x0000 de la EEPROM

La capacidad de la EEPROM es de 16Kbytes.

Ejemplo lectura y escritura de un diccionario usando JSON:
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

En este ejemplo se crea un dicionario python y luego se serializa utilizando el modulo json, una vez obtenido el string que representa al diccionario, se graba
en la EEPROM mediante el metodo "write". 
Para volver a construir el diccionario, se lee el string desde la EEPROM mediante el metodo "readall" y luego se decodifica mediante el metodo "loads" del modulo json.




##### Soporte para SPI modo Master

Clase `pyb.SPI`.

Ejemplo lectura y escritura sobre el modulo SPI (half-duplex):
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
El constructor de la clase SPI recibe 3 argumentos:

- cantidad de bits: 4 8 o 16
- Modo del SPI: 0,1,2 o 3
- Frecuencia del clock SPI: expresada en Hz

Una vez creado el objeto spi se podra leer y escribir datos mediante el metodo write, el cual recibe un bytearray y el metodo read el cual recibe como argumento la cantidad
de frames spi (cantidad de bytes si los bits fueron configurados en 8) y devuelve un array de bytes con los datos leidos.


##### Soporte para RTC

Clase `pyb.RTC`.

Ejemplo:
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

El metodo "datetime" lee o setea los valores de fecha y hora del modulo RTC. Si no se le pasan argumentos, el metodo devolvera uan tupla de 7 valores con 
la fecha y hora actual. Si se la pasa una tupla igual como argumento, esos valores se cargan en el modulo RTC.
El formato de la tupla es:
(year, month, day, weekday, hours, minutes, seconds)

El campo weekday toma los valores de 0 a 6
El modulo RTC continua funcionando despues de un reset del CPU y si se alimenta al mismo con una bateria, el RTC seguira funcionando inclusive sin la
alimentacion principal del CPU.


Ejemplo de calibracion:
```python
import pyb
rtc = pyb.RTC()
rtc.calibration(0)
newDt = [2015,12,31,0,18,16,0]
rtc.datetime(newDt)
```

El metodo "calibration" ajusta de forma periodica el contador del modulo RTC
los valores permitidos son -131072 to 131072. Leer http://www.nxp.com/documents/user_manual/UM10503.pdf para informacion detallada del procedimiento de calibracion.
Luego de la calibracion se debe setear una fecha y hora.


Ejemplo uso de registros de backup:
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

Existen 64 registros de 32bits que mantendran sus valores a pesar de que se reinicie el CPU y si se alimenta al mismo con una bateria, 
se mantendran los valores inclusive sin la alimentacion principal del CPU.

El metodo "write_bkp_reg" tiene como argumento la direccion del registro (0 a 63) y el valor de 32bit a escribir.
El metodo "read_bkp_reg" tiene como argumento la direccion del registro (0 a 63) y devolvera el valor que se encuentra en el mismo.


Ejemplo uso de alarma:
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

En este ejemplo se define una funcion (rtcCallback) que se ejecutara cuando se cumpla la fecha de la alarma. Mediante el metodo "alarm_datetime"
se configura (o lee) la fecha de la alarma con la misma tupla de 7 valores usada para configurar la fecha y hora actual, y como segundo argumento se le pasa
la mascara de la alarma la cual se construye con las constantes:

- pyb.RTC.MASK_SEC : Se chequea el campo de segundos en la comparacion.
- pyb.RTC.MASK_MIN : Se chequea el campo de minutos en la comparacion.
- pyb.RTC.MASK_HR : Se chequea el campo de horas en la comparacion.
- pyb.RTC.MASK_DAY : Se chequea el campo de dia en la comparacion.
- pyb.RTC.MASK_MON : Se chequea el campo de mes en la comparacion.
- pyb.RTC.MASK_YR : Se chequea el campo de año en la comparacion.
- pyb.RTC.MASK_DOW : Se chequea el campo de dia de la semana en la comparacion.

Mediante el metodo "callback" se setea la funcion que se ejecutara cuando se produzca la alarma. En el ejemplo se construye la mascara con segundos y minutos,
por lo que la alarma se producira cada hora, a los 16 minutos y 10 segundos.

Para deshabilitar la alarma puede ejecutarse el metodo "alarm_disable"


## Modulo para soporte de operaciones de fecha y hora

### Existe el soporte de funciones temporales mediante el modulo utime.

Ejemplo:
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

- localtime([secs]): Convierte un timestamp desde el 1 de enero de 2000 a una tupla de 8 items (year, month, mday, hour, minute, second, weekday, yearday)
Si no se le pasan los segundos como argumento, se utilizara la fech ay hora actual tomadas del modulo RTC. El año incluye todos los digitos (p. ej. 2016)
el mes es de 1 a 12 y el dia de 1 a 31, las horas de 0 a 23, los minutos de 0 a 59 asi como los segundos. el dia de la semana va de 0 a 6 y el dia del año
de 1 a 366.

- mktime(): Es la funcion inversa a localtime. Recibe como argumento una tupla de 8 items y devuelve la cantidad de segundos desde el 1 de enero de 2000.

- sleep(seconds): Es un delay de la cantidad de segundos pasada como argumento, el cual puede ser un float.

- time(): Devuelve el timestamp en segundos desde el 1 de enero de 2000, tomando la fecha y hora del modulo RTC


NOTA: El modulo RTC debe ser inicializado antes de utilizar el modulo utime.

> Mas informacion en: https://micropython.org/doc/module/time



##### Soporte para I2C modo Master

Clase `pyb.I2C`.

Ejemplo lectura y escritura de una memoria tipo 24C04:
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
El constructor de la clase I2C recibe como argumento la velocidad (100Khz o 400Khz) expresada en Hz:
Luego de crear el objeto i2c, se ejecuta el metodo slave_addr mediante el cual se configura la direccion del dispositivo esclavo que se
utilizara al leer y escribir datos por el bus. Puede cambiarse en cualquier momento.

Se podra leer y escribir datos mediante el metodo write, el cual recibe un bytearray y el metodo read el cual recibe como argumento la cantidad
de bytes a leer y devuelve un array de bytes con los datos leidos.

En el ejemplo se escriben dos funciones para poder escribir un byte en la memoria y luego poder leer una cierta cantidad de bytes, siempre
indicando la direccion.


## Modulos escritos en python

## Modbus module

### Modbus master con la clase Instrument.

Ejemplo: Lectura de registro en la direccion 0x0001 desde un slave con la direccion 0x55 usando la uart 3

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

La clase Instrument necesita para ser creada, un objeto uart, que representa un puerto serie de la placa, la direccion del slave
(0x55 en este ejemplo) y por ultimo el modo del protocolo modbus:
- ModBus.MODE_ASCII
- ModBus.MODE_RTU

El objeto instrument se puede usar para leery escribir registros a traves del puerto serie en el slave definido usando el protocolo
Modbus.

Metodos soportados:

- read_bit(self, registeraddress, functioncode=2)
Lee un bit del dispositivo slave
        Args:
            * registeraddress (int): La direccion del registro del slave.
            * functioncode (int): Codigo de funcion modbus. Puede ser 1 o 2.

        Returns:
			El valor del bit 0 o 1 (int).

        Raises:
            ValueError, TypeError, IOError


- write_bit(self, registeraddress, value, functioncode=5)
Escribe un bit en el dispositivo slave.
        Args:
            * registeraddress (int): La direccion del registro del slave.
            * value (int): 0 o 1
            * functioncode (int): Codigo de funcion modbus. 5 o 15.

        Returns:
            None

        Raises:
            ValueError, TypeError, IOError


- read_register(self, registeraddress, numberOfDecimals=0, functioncode=3, signed=False)
Lee un integer de uno de los registros de 16 bit del dispositivo slave, con la posibilidad de escalarlo.

        Args:
            * registeraddress (int): La direccion del registro del slave.
            * numberOfDecimals (int): Cantidad de decimales para la conversion del contenido.
            * functioncode (int): Codigo de funcion modbus. 3 o 4.
            * signed (bool): Interpretacion del contenido con signo.

		Si un valor de 77.0 se almacena internamente en el registro como 770, se debe utilizar ``numberOfDecimals=1``
		de este modo se dividira el contenido por 10 antes de devolver el valor. 

		De la misma forma, si se utiliza ``numberOfDecimals=2`` se dividira por 100.

        Returns:
            El dato del registro en forma numerica (int o float).

        Raises:
            ValueError, TypeError, IOError


- write_register(self, registeraddress, value, numberOfDecimals=0, functioncode=16, signed=False)
Escribe un integer de uno de los registros de 16 bit del dispositivo slave, con la posibilidad de escalarlo.

        Args:
            * registeraddress (int): La direccion del registro del slave.
            * value (int or float): Valor a escribir.
            * numberOfDecimals (int): Cantidad de decimales para la conversion del contenido.
            * functioncode (int): Codigo de funcion modbus. 6 o 16.
            * signed (bool): Interpretacion del contenido con signo.

		Si un valor de 77.0 se almacena internamente en el registro como 770, se debe utilizar ``numberOfDecimals=1``
		de este modo se multiplicara el contenido por 10 antes de enviar el valor al slave. 

        Returns:
            None

        Raises:
            ValueError, TypeError, IOError


- read_registers(self, registeraddress, numberOfRegisters, functioncode=3)
Lee integers en los registros de 16 bit del dispositivo slave

        Args:
            * registeraddress (int): La direccion de inicio de los registros a leer del slave.
            * numberOfRegisters (int): Numero de registros a leer.
            * functioncode (int): Codigo de funcion modbus. 3 o 4.

        Returns:
            Los valores de los registros en una list de integers.

        Raises:
            ValueError, TypeError, IOError


- write_registers(self, registeraddress, values)
Escribe integers en los registros de 16-bit del dispositivo slave.

		Usa el codigo de funcion modbus 16.

		El numero de registros a escribir esta dado por la cantidad de valores en la lista.

        Args:
            * registeraddress (int): La direccion de inicio de los registros a escribir en el slave.
            * values (list of int): Valores a escribir en los registros.

        Returns:
            None

        Raises:
            ValueError, TypeError, IOError



### Soporte de Modbus slave mediante la clase Slave.

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

En el modo slave, se necesita definir un diccionario donde se mapearan los supuestos registros del dispositivo slave.
En este diccionario, las claves seran las direcciones de los registros, y los valores del diccionario,los valores de cada registro.
En el ejemplo, se definen dos registros, uno en la direccion 0x4000 y otro en la direccion 0x4001 ambos con el valor 0.
Se soportan comandos para leer y escribir registros, estos comandos modificaran los valores del diccionario.

Para crear el objeto Slave, se necesita un objeto uart, la direccion del slave, el diccionario con los registros definidos y la constante
que indica se se utilizara Modbus en modo ascii o rtu.

El metodo "receive" se debe ejecutar costantemente para recibir por el puerto serie las tramas modbus, al recibirlas se modificara el 
estado del diccionario y se devolvera True, de lo contrario el metodo devolvera False.






