# Test StrEnum class
from enum import StrEnum

print("Test 1: Basic StrEnum")
class Color(StrEnum):
    RED = "red"
    GREEN = "green"
    BLUE = "blue"

print(f"Color.RED = {Color.RED}")
assert Color.RED == "red"
assert str(Color.RED) == "red"
print("PASS")

print("\nTest 2: String operations")
result = Color.RED.upper()
print(f"Color.RED.upper() = {result}")
assert result == "RED"
print("PASS")

print("\nTest 3: String concatenation")
result = Color.RED + "_color"
print(f"Color.RED + '_color' = {result}")
assert result == "red_color"
print("PASS")

print("\nTest 4: Enum properties still work")
print(f"Color.RED.name = {Color.RED.name}")
print(f"Color.RED.value = {Color.RED.value}")
assert Color.RED.name == "RED"
assert Color.RED.value == "red"
print("PASS")

print("\nTest 5: Lookup by value")
c = Color("red")
print(f"Color('red') is Color.RED: {c is Color.RED}")
assert c is Color.RED
print("PASS")

print("\nTest 6: StrEnum rejects non-string values")
try:
    class Bad(StrEnum):
        NUM = 123
    print("FAIL - should have raised TypeError")
except TypeError as e:
    print(f"Correctly raised TypeError: {e}")
    print("PASS")

print("\nAll StrEnum tests passed!")
