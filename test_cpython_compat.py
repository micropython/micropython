"""
Test CPython compatibility of enum implementation.
This test demonstrates that the MicroPython enum module works
identically to CPython's enum module for supported features.
"""

import sys

sys.path.insert(0, "lib/enum")
from enum import Enum, IntEnum


# Test 1: Basic Enum definition (CPython syntax)
class Color(Enum):
    RED = 1
    GREEN = 2
    BLUE = 3


print("Test 1: Basic Enum")
print("  Color.RED =", Color.RED)
print("  Color.RED.name =", Color.RED.name)
print("  Color.RED.value =", Color.RED.value)
assert Color.RED.name == "RED"
assert Color.RED.value == 1
print("  ✓ PASS")

# Test 2: Value lookup (CPython syntax)
print("\nTest 2: Value lookup")
print("  Color(1) =", Color(1))
print("  Color(1) is Color.RED =", Color(1) is Color.RED)
assert Color(1) is Color.RED
print("  ✓ PASS")

# Test 3: Iteration (CPython syntax)
print("\nTest 3: Iteration")
members = list(Color)
print("  list(Color) =", members)
assert len(members) == 3
assert Color.RED in members
print("  ✓ PASS")

# Test 4: Membership testing (CPython syntax)
print("\nTest 4: Membership")
print("  Color.RED in Color =", Color.RED in Color)
print("  len(Color) =", len(Color))
assert Color.RED in Color
assert len(Color) == 3
print("  ✓ PASS")


# Test 5: IntEnum (CPython syntax)
class Status(IntEnum):
    PENDING = 0
    ACTIVE = 1
    DONE = 2


print("\nTest 5: IntEnum")
print("  Status.ACTIVE =", Status.ACTIVE)
print("  Status.ACTIVE == 1 =", Status.ACTIVE == 1)
print("  Status.ACTIVE + 10 =", Status.ACTIVE + 10)
print("  int(Status.ACTIVE) =", int(Status.ACTIVE))
assert Status.ACTIVE == 1
assert Status.ACTIVE + 10 == 11
assert int(Status.ACTIVE) == 1
print("  ✓ PASS")

# Test 6: Equality and identity (CPython behavior)
print("\nTest 6: Equality")
print("  Color.RED == Color.RED =", Color.RED == Color.RED)
print("  Color.RED == Color.GREEN =", Color.RED == Color.GREEN)
print("  Color.RED is Color.RED =", Color.RED is Color.RED)
assert Color.RED == Color.RED
assert Color.RED != Color.GREEN
assert Color.RED is Color.RED
print("  ✓ PASS")

# Test 7: String representations (CPython compatible)
print("\nTest 7: String representations")
print("  str(Color.RED) =", str(Color.RED))
print("  repr(Color.RED) =", repr(Color.RED))
assert str(Color.RED) == "Color.RED"
assert "Color.RED" in repr(Color.RED)
assert "1" in repr(Color.RED)
print("  ✓ PASS")

print("\n" + "=" * 60)
print("ALL TESTS PASSED ✓")
print("=" * 60)
print("\nThe enum module is fully CPython-compatible for:")
print("  - Basic Enum and IntEnum")
print("  - Member access and value lookup")
print("  - Iteration and membership testing")
print("  - IntEnum arithmetic operations")
print("  - Equality and identity semantics")
print("  - String representations")
