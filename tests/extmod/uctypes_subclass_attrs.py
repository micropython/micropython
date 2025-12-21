try:
    import uctypes
except ImportError:
    print("SKIP")
    raise SystemExit

# Test that subclassing a native type with an attr handler properly
# delegates attribute stores to the native attr handler instead of
# storing them in the instance dict.

desc = {"value": uctypes.UINT32 | 0}

import array

data = array.array("I", [0])
addr = uctypes.addressof(data)

# Test 1: Native type baseline - handler processes successfully
s = uctypes.struct(addr, desc, uctypes.NATIVE)
s.value = 42
print(data[0])

# Reset
data[0] = 0


# Test 2: Subclass delegates to native attr handler
class MyStruct(uctypes.struct):
    pass


s2 = MyStruct(addr, desc, uctypes.NATIVE)
s2.value = 99
print(data[0])

# Verify value is not in instance dict
try:
    print("value" in s2.__dict__)
except AttributeError:
    # __dict__ might not exist, that's fine
    print(False)

# Test 3: Python attributes in subclass (KeyError fallback path)
data[0] = 0
s2.extra = "test"  # Triggers KeyError in native handler, falls back to dict
s2.value = 123  # native attr still works
print(data[0])
print(s2.extra)
print("extra" in s2.__dict__)

# Test 4: TypeError from native handler is re-raised (RE-RAISE PATH)
try:
    s2.value = "invalid"  # uctypes raises TypeError for wrong type → should be re-raised
except TypeError:
    print("TypeError")

# Test 5: Verify everything still works after exception
data[0] = 0
s2.value = 789
print(data[0])
print(s2.value)

# Test 6: Multiple successful stores to ensure handled=true path is taken
for i in [111, 222, 333]:
    s2.value = i
    print(data[0])

# Test 7: Ensure fall-through path works (KeyError → handled=false → dict storage)
# This explicitly tests the code path after "if (handled)" when handled=false
s2.another_python_attr = "fallthrough"
print("another_python_attr" in s2.__dict__)
