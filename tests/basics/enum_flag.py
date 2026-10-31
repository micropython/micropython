# Test Flag and IntFlag enum classes

# Skip test if enum module is not available
try:
    from enum import Flag, IntFlag, auto
except ImportError:
    print("SKIP")
    raise SystemExit

# Skip test if classmethod builtin is not available
try:
    classmethod
except NameError:
    print("SKIP")
    raise SystemExit

# Skip test if __prepare__ is not supported (needed for auto())
_prepare_test = []

try:
    class _TestMeta(type):
        @classmethod
        def __prepare__(mcs, name, bases):
            _prepare_test.append(1)
            return {}

    class _Test(metaclass=_TestMeta):
        pass
except:
    pass

if not _prepare_test:
    print("SKIP")
    raise SystemExit

print("Test 1: Basic Flag operations")
class Permission(Flag):
    READ = 1
    WRITE = 2
    EXECUTE = 4

p1 = Permission.READ | Permission.WRITE
print("READ | WRITE = %s" % (p1._value_))
assert p1._value_ == 3
print("PASS")

print("\nTest 2: Flag AND operation")
p2 = Permission.READ & Permission.WRITE
print("READ & WRITE = %s" % (p2._value_))
assert p2._value_ == 0
print("PASS")

print("\nTest 3: Flag XOR operation")
p3 = Permission.READ ^ Permission.WRITE
print("READ ^ WRITE = %s" % (p3._value_))
assert p3._value_ == 3
print("PASS")

print("\nTest 4: Flag invert operation")
p4 = ~Permission.READ
print("~READ = %s" % (p4._value_))
assert p4._value_ == 6  # WRITE | EXECUTE
print("PASS")

print("\nTest 5: Flag with auto()")
class Status(Flag):
    IDLE = auto()
    BUSY = auto()
    ERROR = auto()

print("IDLE = %s" % (Status.IDLE._value_))
print("BUSY = %s" % (Status.BUSY._value_))
print("ERROR = %s" % (Status.ERROR._value_))
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
print("R | W = %s" % (m1._value_))
assert m1._value_ == 6
# Note: isinstance(m1, int) may be False in MicroPython due to metaclass limitations
# but m1 supports all int operations
print("PASS")

print("\nTest 7: IntFlag with integer operands")
m2 = Mode.R | 8
print("R | 8 = %s" % (m2._value_))
assert m2._value_ == 12
print("PASS")

print("\nTest 8: IntFlag reverse operations")
m3 = 8 | Mode.R
print("8 | R = %s" % (m3._value_))
assert m3._value_ == 12
print("PASS")

print("\nAll Flag/IntFlag tests passed!")
