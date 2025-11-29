# Test enum.auto() support (requires MICROPY_PY_METACLASS_PREPARE)

# Skip test if enum module is not available
try:
    from enum import Enum, IntEnum, auto
except ImportError:
    print("SKIP")
    raise SystemExit

# Skip test if classmethod builtin is not available
try:
    classmethod
except NameError:
    print("SKIP")
    raise SystemExit

# Skip test if __prepare__ is not supported
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

# Test 1: Basic auto() usage - sequential values starting from 1
print("Test 1: Basic auto() usage")
class Color(Enum):
    RED = auto()
    GREEN = auto()
    BLUE = auto()

print("RED.value = %s" % (Color.RED.value))
print("GREEN.value = %s" % (Color.GREEN.value))
print("BLUE.value = %s" % (Color.BLUE.value))
print("PASS" if (Color.RED.value == 1 and Color.GREEN.value == 2 and Color.BLUE.value == 3) else "FAIL")

# Test 2: Mixed auto() and explicit values
# Note: MicroPython's simplified auto() assigns values after max explicit value
print("\nTest 2: Mixed auto() and explicit values")
class Status(Enum):
    PENDING = auto()
    ACTIVE = 10
    PAUSED = auto()
    STOPPED = auto()

print("PENDING.value = %s" % (Status.PENDING.value))
print("ACTIVE.value = %s" % (Status.ACTIVE.value))
print("PAUSED.value = %s" % (Status.PAUSED.value))
print("STOPPED.value = %s" % (Status.STOPPED.value))
# In MicroPython, auto() starts at max(explicit) + 1, then assigns in creation order
# PENDING, PAUSED, STOPPED were created in that order, so they get 11, 12, 13
print("PASS" if (Status.ACTIVE.value == 10 and
                   Status.PENDING.value == 11 and Status.PAUSED.value == 12 and
                   Status.STOPPED.value == 13) else "FAIL")

# Test 3: auto() with IntEnum
print("\nTest 3: auto() with IntEnum")
class Priority(IntEnum):
    LOW = auto()
    MEDIUM = auto()
    HIGH = auto()

print("LOW.value = %s" % (Priority.LOW.value))
print("MEDIUM.value = %s" % (Priority.MEDIUM.value))
print("HIGH.value = %s" % (Priority.HIGH.value))
print("PASS" if (Priority.LOW.value == 1 and Priority.MEDIUM.value == 2 and Priority.HIGH.value == 3) else "FAIL")

# Test 4: auto() with single member
print("\nTest 4: auto() with single member")
class Single(Enum):
    ONLY = auto()

print("ONLY.value = %s" % (Single.ONLY.value))
print("PASS" if Single.ONLY.value == 1 else "FAIL")

# Test 5: All auto() values
print("\nTest 5: All auto() values")
class AllAuto(Enum):
    A = auto()
    B = auto()
    C = auto()
    D = auto()
    E = auto()

values = [AllAuto.A.value, AllAuto.B.value, AllAuto.C.value, AllAuto.D.value, AllAuto.E.value]
print("Values: %s" % (values))
print("PASS" if values == [1, 2, 3, 4, 5] else "FAIL")

# Test 6: auto() after explicit value 0
print("\nTest 6: auto() after explicit value 0")
class StartZero(Enum):
    ZERO = 0
    ONE = auto()
    TWO = auto()

print("ZERO.value = %s" % (StartZero.ZERO.value))
print("ONE.value = %s" % (StartZero.ONE.value))
print("TWO.value = %s" % (StartZero.TWO.value))
print("PASS" if (StartZero.ZERO.value == 0 and StartZero.ONE.value == 1 and StartZero.TWO.value == 2) else "FAIL")

# Test 7: auto() continues from highest explicit value
print("\nTest 7: auto() continues from highest value")
class Complex(Enum):
    A = auto()
    B = 100
    C = auto()
    D = 50
    E = auto()

print("A.value = %s" % (Complex.A.value))
print("B.value = %s" % (Complex.B.value))
print("C.value = %s" % (Complex.C.value))
print("D.value = %s" % (Complex.D.value))
print("E.value = %s" % (Complex.E.value))
# In MicroPython: auto() starts at max(100, 50) + 1 = 101
# A, C, E created in that order get 101, 102, 103
print("PASS" if (Complex.B.value == 100 and Complex.D.value == 50 and
                   Complex.A.value == 101 and Complex.C.value == 102 and Complex.E.value == 103) else "FAIL")

# Test 8: Verify enum members work correctly with auto()
print("\nTest 8: Enum member functionality with auto()")
class Direction(Enum):
    NORTH = auto()
    SOUTH = auto()
    EAST = auto()
    WEST = auto()

# Test iteration
directions = [d for d in Direction]
print("Member count: %s" % (len(directions)))
print("PASS" if len(directions) == 4 else "FAIL")

# Test value lookup
try:
    north = Direction(1)
    print("Direction(1) = %s" % (north.name))
    print("PASS" if north == Direction.NORTH else "FAIL")
except ValueError:
    print("FAIL - value lookup failed")

# Test 9: auto() repr
print("\nTest 9: auto() repr")
a = auto()
print("repr(auto()) = %s" % (repr(a)))
print("PASS" if repr(a) == "auto()" else "FAIL")

print("\nAll enum.auto() tests completed!")
