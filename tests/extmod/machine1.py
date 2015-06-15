# test machine module

try:
    import machine
except ImportError:
    print("SKIP")
    import sys
    sys.exit()

import uctypes

print(machine.mem8)

buf = bytearray(8)
addr = uctypes.addressof(buf)

machine.mem8[addr] = 123
print(machine.mem8[addr])

machine.mem16[addr] = 12345
print(machine.mem16[addr])

machine.mem32[addr] = 123456789
print(machine.mem32[addr])

try:
    machine.mem16[1]
except ValueError:
    print("ValueError")

try:
    machine.mem16[1] = 1
except ValueError:
    print("ValueError")

try:
    del machine.mem8[0]
except TypeError:
    print("TypeError")
