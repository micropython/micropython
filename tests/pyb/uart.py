from pyb import UART

uart = UART(1)
uart = UART(1, 9600)
uart = UART(1, 9600, bits=8, parity=None, stop=1)
print(uart)

uart.init(2400)
print(uart)

print(uart.any())
print(uart.write('123'))
print(uart.write(b'abcd'))
print(uart.writechar(1))

# make sure this method exists
uart.sendbreak()
