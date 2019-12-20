from pyb import UART

# test we can correctly create by id
for bus in (-1, 0, 1, 2, 5, 6):
    try:
        UART(bus, 9600)
        print("UART", bus)
    except ValueError:
        print("ValueError", bus)

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

# non-blocking mode
uart = UART(1, 9600, timeout=0)
print(uart.write(b'1'))
print(uart.write(b'abcd'))
print(uart.writechar(1))
print(uart.read(100))

# set rxbuf
uart.init(9600, rxbuf=8)
print(uart)
uart.init(9600, rxbuf=0)
print(uart)

# set read_buf_len (legacy, use rxbuf instead)
uart.init(9600, read_buf_len=4)
print(uart)
uart.init(9600, read_buf_len=0)
print(uart)
