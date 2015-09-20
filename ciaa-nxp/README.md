Soporte para EDU-CIAA
========================
<p align="center">
  <img src="https://avatars0.githubusercontent.com/u/6998305?v=3&s=400" alt="CIAA Logo"/>
</p>

### Modo de uso:
- Editar py/Main.py y escribir el programa en Python
- Ejecutar make download para compilar y bajar el programa en la EDU-CIAA. En el makefile se lanzara la herramienta py2c.py que generara el archivo ProgramScript.c con el script de Python.

### Archivos incluidos:
- py/ -- Aqui se encuentra el archivo Main.py donde escribiremos nuestro programa Python.

- py2c.py -- Scripy en Python que convierte el script de Python Main.py en el archivo ProgramScript.c

- ProgramScript.c -- Archivo .C que se incluye en Main.c y contiene como array de bytes el script Python.


## Soporte de hardware

Modulo pyb:
- Soporte de los 4 LEDS de la placa mediante el modulo pyb.LED. Ejemplo:

```python
import pyb
led1 = pyb.LED(1)
led1.on()
pyb.delay(100);
led1.off()
```
Numeros de leds disponibles: de 1 a 4
Mas info en : http://test-ergun.readthedocs.org/en/latest/library/pyb.LED.html

- Soporte para los 4 pulsadores de la placa mediante el modulo pyb.Switch. Ejemplo:

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


Numeros de switch disponibles:  de 1 a 4
Mas info en : http://test-ergun.readthedocs.org/en/latest/library/pyb.Switch.html

- Soporte para la UART RS232 del conector P1 y la UART RS485 de la placa mediante el modulo pyb.UART. Ejemplo:

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

