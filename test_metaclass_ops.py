#!/usr/bin/env python3
"""Test metaclass special methods (__len__, __contains__)"""

import sys

sys.path.insert(0, ".")

from enum_minimal import Enum, IntEnum


class Color(Enum):
    RED = 1
    GREEN = 2
    BLUE = 3


class Status(IntEnum):
    PENDING = 0
    RUNNING = 1
    COMPLETE = 2


print("Test 1: len() on Enum class")
try:
    result = len(Color)
    print(f"len(Color) = {result}")
except Exception as e:
    print(f"ERROR: {e}")

print("\nTest 2: in operator on Enum class")
try:
    result = Color.RED in Color
    print(f"Color.RED in Color = {result}")
except Exception as e:
    print(f"ERROR: {e}")

print("\nTest 3: len() on IntEnum class")
try:
    result = len(Status)
    print(f"len(Status) = {result}")
except Exception as e:
    print(f"ERROR: {e}")

print("\nTest 4: in operator on IntEnum class")
try:
    result = Status.RUNNING in Status
    print(f"Status.RUNNING in Status = {result}")
except Exception as e:
    print(f"ERROR: {e}")

print("\nAll tests completed!")
