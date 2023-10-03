# test suite for SoftUART

from machine import SoftUART, Pin
import time
import select

poll = select.poll()

s1 = SoftUART(Pin(2), Pin(4), baudrate=9600, timeout=0)  # tx=2 rx=4
s2 = SoftUART(Pin(0), Pin(5), baudrate=9600, timeout=0)  # tx=0 rx=5
poll.register(s1)
poll.register(s2)

print("polling...")

while True:
    read = poll.ipoll()

    for r, ev in read:
        if (ev & select.POLLIN) != 0:
            i = None
            if r is s1:
                i = "s1"
            else:
                i = "s2"
            print(i, r.read())
