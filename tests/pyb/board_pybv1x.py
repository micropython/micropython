# Test board-specific items on PYBv1.x

import os, pyb

if not 'PYBv1.' in os.uname().machine:
    print('SKIP')
    raise SystemExit

# test creating UART by id/name
for bus in (1, 2, 3, 4, 5, 6, 7, "XA", "XB", "YA", "YB", "Z"):
    try:
        pyb.UART(bus, 9600)
        print("UART", bus)
    except ValueError:
        print("ValueError", bus)

# test creating SPI by id/name
for bus in (1, 2, 3, "X", "Y", "Z"):
    try:
        pyb.SPI(bus)
        print("SPI", bus)
    except ValueError:
        print("ValueError", bus)

# test creating I2C by id/name
for bus in (2, 3, "X", "Y", "Z"):
    try:
        pyb.I2C(bus)
        print("I2C", bus)
    except ValueError:
        print("ValueError", bus)

# test creating CAN by id/name
for bus in (1, 2, 3, "YA", "YB", "YC"):
    try:
        pyb.CAN(bus, pyb.CAN.LOOPBACK)
        print("CAN", bus)
    except ValueError:
        print("ValueError", bus)
