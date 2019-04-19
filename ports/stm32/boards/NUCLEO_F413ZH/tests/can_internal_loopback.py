# MicroPython CAN loopback test.
# This test works on all boards with one or more CAN port.
# No external hardware is required.
# Set which CAN controllers to test in "available_CANs".

# (C) 2019 Chris Mason
# Licenced under the terms of the MicroPython MIT licence.
# https://github.com/micropython/micropython/blob/master/LICENSE

import pyb
from pyb import CAN

# -----------------------------
available_CANs = [1, 2, 3]
# -----------------------------

cans = []

for i in range(0, max(available_CANs)):
    cans.append('none')
    if (i+1) in available_CANs:
        cans[i] = CAN(i+1, CAN.LOOPBACK)
        cans[i].setfilter(0, CAN.LIST16, 0, (123, 124, 125, 126))   # set a filter to receive messages with id=123, 124, 125 and 126

def CAN_Send(s):
    message = 'CAN ' + str(s) + '->'
    cans[s-1].send(message, 123)                            # send a message with id 123

def CAN_Recv(r):
    res = cans[r-1].recv(0, timeout=50)                     # receive message on FIFO 0
    return res

try:
    while True:
        for i in available_CANs:
            CAN_Send(i)
            pyb.delay(100)
            res = CAN_Recv(i)
            print(res[3].decode("utf-8") + str(i), " passed internal loopback")
        print('----------------------------------')

finally:
    print('FAIL, loop ', i, '\n')
