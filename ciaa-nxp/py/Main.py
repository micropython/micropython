import pyb

print("Abro uart 3")
uart = pyb.UART(3)
print("Abri uart 3")

uart.init(115200,bits=8, parity=None, stop=1,timeout=1000, timeout_char=0, read_buf_len=64)

while True:
	if uart.any():
		print("hay data:")
		d = uart.readchar()
          	print(str(d))
		break

