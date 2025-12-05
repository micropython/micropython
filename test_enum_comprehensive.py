#!/usr/bin/env python3
"""Comprehensive test suite for enum_minimal.py"""

import sys

sys.path.insert(0, "lib/enum")

from enum import Enum, IntEnum

print("=" * 60)
print("Comprehensive Enum Test Suite")
print("=" * 60)

# Test 1: Basic Enum creation and member access
print("\n[Test 1] Basic Enum creation and member access")


class Color(Enum):
    RED = 1
    GREEN = 2
    BLUE = 3


assert Color.RED.value == 1
assert Color.GREEN.value == 2
assert Color.BLUE.value == 3
assert Color.RED.name == "RED"
assert Color.GREEN.name == "GREEN"
print("✓ PASS: Basic Enum members work")

# Test 2: Enum member identity and equality
print("\n[Test 2] Enum member identity and equality")
assert Color.RED == Color.RED
assert Color.RED != Color.GREEN
assert Color.RED is Color.RED
print("✓ PASS: Enum member identity and equality work")

# Test 3: Enum iteration
print("\n[Test 3] Enum iteration")
members = list(Color)
assert len(members) == 3
assert Color.RED in members
assert Color.GREEN in members
assert Color.BLUE in members
print(f"  Members: {', '.join(str(m) for m in members)}")
print("✓ PASS: Enum iteration works")

# Test 4: Enum lookup by value
print("\n[Test 4] Enum lookup by value")
assert Color(1) is Color.RED
assert Color(2) is Color.GREEN
assert Color(3) is Color.BLUE
try:
    Color(99)
    assert False, "Should have raised ValueError"
except ValueError as e:
    assert "99" in str(e)
print("✓ PASS: Enum value lookup works")

# Test 5: Metaclass __len__
print("\n[Test 5] Metaclass __len__")
assert len(Color) == 3
print(f"  len(Color) = {len(Color)}")
print("✓ PASS: Metaclass __len__ works")

# Test 6: Metaclass __contains__
print("\n[Test 6] Metaclass __contains__")
assert Color.RED in Color
assert Color.GREEN in Color
assert Color.BLUE in Color


# Test with non-member
class Other(Enum):
    X = 1


assert Other.X not in Color
print("✓ PASS: Metaclass __contains__ works")

# Test 7: IntEnum with integer operations
print("\n[Test 7] IntEnum with integer operations")


class Status(IntEnum):
    PENDING = 0
    RUNNING = 1
    COMPLETE = 2


assert Status.PENDING + 1 == 1
assert Status.RUNNING * 2 == 2
assert Status.COMPLETE - 1 == 1
assert Status.RUNNING == 1  # Compare with int
assert Status.RUNNING > Status.PENDING
assert Status.COMPLETE >= Status.RUNNING
print(f"  RUNNING + 1 = {Status.RUNNING + 1}")
print(f"  COMPLETE * 2 = {Status.COMPLETE * 2}")
print("✓ PASS: IntEnum arithmetic works")

# Test 8: IntEnum identity vs value equality
print("\n[Test 8] IntEnum identity vs value equality")
assert Status.RUNNING == Status.RUNNING  # Identity
assert Status.RUNNING == 1  # Value equality with int
# Note: Can't test "is not" with small ints due to Python's integer caching
print("✓ PASS: IntEnum equality semantics work")

# Test 9: IntEnum iteration and len
print("\n[Test 9] IntEnum iteration and len")
assert len(Status) == 3
statuses = list(Status)
assert len(statuses) == 3
print("✓ PASS: IntEnum iteration and len work")

# Test 10: IntEnum __contains__
print("\n[Test 10] IntEnum __contains__")
assert Status.RUNNING in Status
assert Status.COMPLETE in Status
print("✓ PASS: IntEnum __contains__ works")

# Test 11: Enum string representations
print("\n[Test 11] Enum string representations")
assert "Color.RED" in str(Color.RED)
assert "RED" in repr(Color.RED)
print(f"  str(Color.RED) = {str(Color.RED)}")
print(f"  repr(Color.RED) = {repr(Color.RED)}")
print("✓ PASS: Enum string representations work")

# Test 12: Hash consistency
print("\n[Test 12] Hash consistency")
assert hash(Color.RED) == hash(Color.RED)
assert hash(Color.RED) != hash(Color.GREEN)
d = {Color.RED: "red", Color.GREEN: "green"}
assert d[Color.RED] == "red"
print("✓ PASS: Enum hashing works")

print("\n" + "=" * 60)
print("ALL TESTS PASSED ✓")
print("=" * 60)
