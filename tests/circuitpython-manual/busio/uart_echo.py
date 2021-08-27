import busio
import board
import time

i = 0

u = busio.UART(tx=board.TX, rx=board.RX)

while True:
    u.write(str(i).encode("utf-8"))
    time.sleep(0.1)
    print(i, u.in_waiting)  # should be the number of digits
    time.sleep(0.1)
    print(i, u.in_waiting)  # should be the number of digits
    r = u.read(64 + 10)
    print(i, u.in_waiting)  # should be 0
    print(len(r), r)
    i += 1
