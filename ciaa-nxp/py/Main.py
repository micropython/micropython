import pyb

print("Abro uart 3")
uart = pyb.UART(3,115200)
print("Abri uart 3")

#uart.init(115200,bits=8, parity=None, stop=1,timeout=0, timeout_char=1000, read_buf_len=64,packet_mode=True,packet_end_char=ord('o'))
#uart.init(115200,bits=8, parity=None, stop=1,timeout=1000, timeout_char=1000, read_buf_len=64)
#uart.init(115200)

print(uart)

uart.deinit()

while True:
	if uart.any():
		print("hay data:")
		
		# Test 1
		#d = uart.readchar()
          	#print(str(d))
		#uart.writechar(d)
		
		#Test 2
		#data = uart.read()
		data = uart.readall()
		#data = uart.readline()
		#data = uart.read(3)
		
		#buf = bytearray(256)
		#bytes = uart.readinto(buf)
		#if bytes != 0:
		#	print(buf)

		print(data)
		uart.write(data)
