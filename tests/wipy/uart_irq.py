'''
UART IRQ test for the CC3200 based boards.
'''

from machine import UART
import os
import time

mch = os.uname().machine
if 'LaunchPad' in mch:
    uart_pins = [[('GP12', 'GP13'), ('GP12', 'GP13', 'GP7', 'GP6')], [('GP16', 'GP17'), ('GP16', 'GP17', 'GP7', 'GP6')]]
elif 'WiPy' in mch:
    uart_pins = [[('GP12', 'GP13'), ('GP12', 'GP13', 'GP7', 'GP6')], [('GP16', 'GP17'), ('GP16', 'GP17', 'GP7', 'GP6')]]
else:
    raise Exception('Board not supported!')

# just in case we have stdio duplicated on any of the uarts
os.dupterm(None)

uart0 = UART(0, 1000000, pins=uart_pins[0][0])
uart1 = UART(1, 1000000, pins=uart_pins[1][0])

uart0_int_count = 0
uart1_int_count = 0

def uart0_handler (uart_o):
    global uart0_irq
    global uart0_int_count
    if (uart0_irq.flags() & UART.RX_ANY):
        uart0_int_count += 1

def uart1_handler (uart_o):
    global uart1_irq
    global uart1_int_count
    if (uart1_irq.flags() & UART.RX_ANY):
        uart1_int_count += 1

uart0_irq = uart0.irq(trigger=UART.RX_ANY, handler=uart0_handler)
uart1_irq = uart1.irq(trigger=UART.RX_ANY, handler=uart1_handler)

uart0.write(b'123')
# wait for the characters to be received
while not uart1.any():
    pass

time.sleep_us(100)
print(uart1.any() == 3)
print(uart1_int_count > 0)
print(uart1_irq.flags() == 0)
print(uart0_irq.flags() == 0)
print(uart1.read() == b'123')

uart1.write(b'12345')
# wait for the characters to be received
while not uart0.any():
    pass

time.sleep_us(100)
print(uart0.any() == 5)
print(uart0_int_count > 0)
print(uart0_irq.flags() == 0)
print(uart1_irq.flags() == 0)
print(uart0.read() == b'12345')

# do it again
uart1_int_count = 0
uart0.write(b'123')
# wait for the characters to be received
while not uart1.any():
    pass

time.sleep_us(100)
print(uart1.any() == 3)
print(uart1_int_count > 0)
print(uart1_irq.flags() == 0)
print(uart0_irq.flags() == 0)
print(uart1.read() == b'123')

# disable the interrupt
uart1_irq.disable()
# do it again
uart1_int_count = 0
uart0.write(b'123')
# wait for the characters to be received
while not uart1.any():
    pass

time.sleep_us(100)
print(uart1.any() == 3)
print(uart1_int_count == 0) # no interrupt triggered this time
print(uart1_irq.flags() == 0)
print(uart0_irq.flags() == 0)
print(uart1.read() == b'123')

# enable the interrupt
uart1_irq.enable()
# do it again
uart1_int_count = 0
uart0.write(b'123')
# wait for the characters to be received
while not uart1.any():
    pass

time.sleep_us(100)
print(uart1.any() == 3)
print(uart1_int_count > 0)
print(uart1_irq.flags() == 0)
print(uart0_irq.flags() == 0)
print(uart1.read() == b'123')

uart1_irq.init(trigger=UART.RX_ANY, handler=None) # No handler
# do it again
uart1_int_count = 0
uart0.write(b'123')
# wait for the characters to be received
while not uart1.any():
    pass

time.sleep_us(100)
print(uart1.any() == 3)
print(uart1_int_count == 0) # no interrupt handler called
print(uart1_irq.flags() == 0)
print(uart0_irq.flags() == 0)
print(uart1.read() == b'123')

# check for memory leaks
for i in range(0, 1000):
    uart0_irq = uart0.irq(trigger=UART.RX_ANY, handler=uart0_handler)
    uart1_irq = uart1.irq(trigger=UART.RX_ANY, handler=uart1_handler)

# next ones must raise
try:
    uart0_irq = uart0.irq(trigger=100, handler=uart0_handler)
except:
    print('Exception')

try:
    uart0_irq = uart0.irq(trigger=0)
except:
    print('Exception')

try:
    uart0_irq = uart0.irq(trigger=UART.RX_ANY, wake=Sleep.SUSPENDED)
except:
    print('Exception')

uart0_irq.disable()
uart1_irq.disable()
