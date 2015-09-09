import pyb

print("Abro uart 3")
uart = pyb.UART(3)
print("Abri uart 3")

uart.init(115200,bits=8, parity=None, stop=1,timeout=1000, timeout_char=0, read_buf_len=64)

while True:
	if uart.any():
		print("hay data:")
		
		# Test 1
		#d = uart.readchar()
          	#print(str(d))
		#uart.writechar(d)
		
		#Test 2
		data = uart.read()
		#data = uart.readall()
		#data = uart.readline()
		#data = uart.read(3) # NO
		print(data)
		uart.write(data)
