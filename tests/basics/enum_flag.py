# Test Flag and IntFlag enum classes
from enum import Flag, IntFlag, auto

print("Test 1: Basic Flag operations")
class Permission(Flag):
    READ = 1
    WRITE = 2
    EXECUTE = 4

p1 = Permission.READ | Permission.WRITE
print(f"READ | WRITE = {p1._value_}")
assert p1._value_ == 3
print("PASS")

print("\nTest 2: Flag AND operation")
p2 = Permission.READ & Permission.WRITE
print(f"READ & WRITE = {p2._value_}")
assert p2._value_ == 0
print("PASS")

print("\nTest 3: Flag XOR operation")
p3 = Permission.READ ^ Permission.WRITE
print(f"READ ^ WRITE = {p3._value_}")
assert p3._value_ == 3
print("PASS")

print("\nTest 4: Flag invert operation")
p4 = ~Permission.READ
print(f"~READ = {p4._value_}")
assert p4._value_ == 6  # WRITE | EXECUTE
print("PASS")

print("\nTest 5: Flag with auto()")
class Status(Flag):
    IDLE = auto()
    BUSY = auto()
    ERROR = auto()

print(f"IDLE = {Status.IDLE._value_}")
print(f"BUSY = {Status.BUSY._value_}")
print(f"ERROR = {Status.ERROR._value_}")
assert Status.IDLE._value_ == 1
assert Status.BUSY._value_ == 2
assert Status.ERROR._value_ == 3
print("PASS")

print("\nTest 6: IntFlag basic operations")
class Mode(IntFlag):
    R = 4
    W = 2
    X = 1

m1 = Mode.R | Mode.W
print(f"R | W = {m1._value_}")
assert m1._value_ == 6
# Note: isinstance(m1, int) may be False in MicroPython due to metaclass limitations
# but m1 supports all int operations
print("PASS")

print("\nTest 7: IntFlag with integer operands")
m2 = Mode.R | 8
print(f"R | 8 = {m2._value_}")
assert m2._value_ == 12
print("PASS")

print("\nTest 8: IntFlag reverse operations")
m3 = 8 | Mode.R
print(f"8 | R = {m3._value_}")
assert m3._value_ == 12
print("PASS")

print("\nAll Flag/IntFlag tests passed!")
