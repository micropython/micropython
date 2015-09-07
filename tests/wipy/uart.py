'''
UART test fro the CC3200 based boards. 
UART0 and UART1 must be connected together for this test to pass.
'''

from pyb import UART
from pyb import Pin
import os

machine = os.uname().machine
if 'LaunchPad' in machine:
    uart_id_range = range(0, 2)
    uart_pins = [[('GP12', 'GP13'), ('GP12', 'GP13', 'GP7', 'GP6')], [('GP16', 'GP17'), ('GP16', 'GP17', 'GP7', 'GP6')]]
elif 'WiPy' in machine:
    uart_id_range = range(0, 2)
    uart_pins = [[('GP12', 'GP13'), ('GP12', 'GP13', 'GP7', 'GP6')], [('GP16', 'GP17'), ('GP16', 'GP17', 'GP7', 'GP6')]]
else:
    raise Exception('Board not supported!')

for uart_id in uart_id_range:
    uart = UART(uart_id, 38400)
    print(uart)
    uart.init(baudrate=57600, stop=1, parity=None, pins=uart_pins[uart_id][0])
    uart.init(baudrate=9600, stop=2, parity=0, pins=uart_pins[uart_id][1])
    uart.init(baudrate=115200, parity=1, pins=uart_pins[uart_id][0])
    uart.sendbreak()

# assign GP1, GP2, GP3 and GP4 back to GPIO mode
Pin('GP1', mode=Pin.IN)
Pin('GP2', mode=Pin.IN)
Pin('GP3', mode=Pin.IN)
Pin('GP4', mode=Pin.IN)

# now it's time for some loopback tests between the uarts
uart0 = UART(0, 1000000, pins=uart_pins[0][0])
print(uart0)
uart1 = UART(1, 1000000, pins=uart_pins[1][0])
print(uart1)

print(uart0.write(b'123456') == 6)
print(uart1.read() == b'123456')

print(uart1.write(b'123') == 3)
print(uart0.read(1) == b'1')
print(uart0.read(2) == b'23')
print(uart0.read() == b'')

uart0.write(b'123')
buf = bytearray(3)
print(uart1.readinto(buf, 1) == 1) 
print(buf)
print(uart1.readinto(buf) == 2)
print(buf)

uart0.write(b'123')
print(uart1.any() > 0)
print(uart1.readline() == b'123')
print(uart1.any() == 0)

uart0.write(b'1234567890')
print(uart1.readall() == b'1234567890')

# tx only mode
Pin('GP13', mode=Pin.IN)
uart0 = UART(0, 1000000, pins=('GP12', None))
print(uart0.write(b'123456') == 6)
print(uart1.read() == b'123456')
print(uart1.write(b'123') == 3)
print(uart0.read() == b'')

# rx only mode
Pin('GP12', mode=Pin.IN)
uart0 = UART(0, 1000000, pins=(None, 'GP13'))
print(uart0.write(b'123456') == 6)
print(uart1.read() == b'')
print(uart1.write(b'123') == 3)
print(uart0.read() == b'123')

Pin('GP13', mode=Pin.IN)
Pin('GP12', mode=Pin.IN)
# no pin assignemnt
uart0 = UART(0, 1000000, pins=None)
print(uart0.write(b'123456789') == 9)
print(uart1.read() == b'')
print(uart1.write(b'123456789') == 9)
print(uart0.read() == b'')
print(Pin.board.GP12)
print(Pin.board.GP13)

# next ones must raise
try:
    UART(0, 9600, parity=2, pins=('GP12', 'GP13', 'GP7'))
except Exception:
    print('Exception')
try:
    UART(0, 9600, parity=2, pins=('GP12', 'GP7'))
except Exception:
    print('Exception')
