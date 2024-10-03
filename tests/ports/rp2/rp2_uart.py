# Test construction of machine.UART objects.

import sys
from machine import UART

print(UART(0, tx=0, rx=1))

if "RP2350" in sys.implementation._machine:
    # Test that UART can be constructed using other tx/rx pins.
    UART(0, tx=2, rx=3)
