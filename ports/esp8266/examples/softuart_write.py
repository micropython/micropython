# test suite for SoftUART

from machine import SoftUART, Pin
import time

s1 = SoftUART(Pin(2), Pin(4), baudrate=9600, timeout=0) # tx=2 rx=4
s2 = SoftUART(Pin(0), Pin(5), baudrate=9600, timeout=0) # tx=0 rx=5

while True:
	print("sending... ",end="")
	s1.write("Hello world s1\r\n")
	print("1, ", end="")
	s2.write("Hello world s2\r\n")
	print("2, done.")
	time.sleep(1)
