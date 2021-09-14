# test suite for SoftUART

from machine import SoftUART, Pin
import time

s1 = SoftUART(tx=Pin(12), rx=Pin(4), baudrate=9600, timeout=0)
s2 = SoftUART(tx=Pin(0), rx=Pin(5), baudrate=9600, timeout=0)

# Change tx pin and baudrate
s1.init(tx=Pin(2), rx=Pin(4), baudrate=115200)

c = 10
while c > 0:
    print("sending... ", end="")
    s1.write("Hello world s1\r\n")
    print("1, ", end="")
    s2.write("Hello world s2\r\n")
    print("2, done.")
    time.sleep(1)
    c -= 1

s1.deinit()
s2.deinit()
print("done")
