# Test StrEnum class

# Skip test if enum module is not available
try:
    from enum import StrEnum
except ImportError:
    print("SKIP")
    raise SystemExit

print("Test 1: Basic StrEnum")
class Color(StrEnum):
    RED = "red"
    GREEN = "green"
    BLUE = "blue"

print("Color.RED = %s" % (Color.RED))
assert Color.RED == "red"
assert str(Color.RED) == "red"
print("PASS")

print("\nTest 2: String operations")
result = Color.RED.upper()
print("Color.RED.upper() = %s" % (result))
assert result == "RED"
print("PASS")

print("\nTest 3: String concatenation")
result = Color.RED + "_color"
print("Color.RED + '_color' = %s" % (result))
assert result == "red_color"
print("PASS")

print("\nTest 4: Enum properties still work")
print("Color.RED.name = %s" % (Color.RED.name))
print("Color.RED.value = %s" % (Color.RED.value))
assert Color.RED.name == "RED"
assert Color.RED.value == "red"
print("PASS")

print("\nTest 5: Lookup by value")
c = Color("red")
print("Color('red') is Color.RED: %s" % (c is Color.RED))
assert c is Color.RED
print("PASS")

print("\nTest 6: StrEnum rejects non-string values")
try:
    class Bad(StrEnum):
        NUM = 123
    print("FAIL - should have raised TypeError")
except TypeError as e:
    print("Correctly raised TypeError: %s" % (e))
    print("PASS")

print("\nAll StrEnum tests passed!")
