# test machine module

try:
    import machine
except ImportError:
    print("SKIP")
    import sys
    sys.exit()

print(machine.mem8)

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
