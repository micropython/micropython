# MicroPython UART hardware test.
# Set which UARTs to test in "available_UARTs".
# If a UART is used for REPL, eg through STLINK on NUCLEO boards remove it from the list and test the REPL.
# First test each Tx pin connected to an external serial port, eg FTDI cable at 115200 baud. This verifies the baud rate.
# Then connect each UART's Tx to Rx and verify that the message is received.

# (C) 2019 Chris Mason
# Licenced under the terms of the MicroPython MIT licence.
# https://github.com/micropython/micropython/blob/master/LICENSE

import pyb
import machine

# -----------------------------------------------
# NUCLEO_F413ZH ST-LINK REPL is on UART3
available_UARTs = [1, 2, 4, 5, 6, 7, 8, 9, 10]
# -----------------------------------------------

uarts = []

for i in range(0, max(available_UARTs)):
    uarts.append('none')
    if (i+1) in available_UARTs:
        uarts[i] = machine.UART(i+1, 115200, bits=8, parity=None, stop=1)
try:
    while True:
        for i in available_UARTs:
            message = 'UART ' + str(i)
            print('sending  ' , message)
            uarts[i-1].write(message)
            pyb.delay(10)
            res = uarts[i-1].read()
            if res:
                res = res.decode("utf-8")
                if (res == 'UART ' + str(i)):
                    res = res + '     passed'
            print('received ' , res, '\n')
        print('------------------')
finally:
    print('FAIL, UART ', i, '\n')