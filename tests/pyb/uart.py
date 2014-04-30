from pyb import UART

uart = UART(1)
uart = UART(1, 9600)
uart = UART(1, 9600, bits=8, stop=1, parity=None)
print(uart)

uart.init(1200)
print(uart)

uart.any()
uart.send(1, timeout=500)
