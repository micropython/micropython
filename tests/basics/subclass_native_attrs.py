# Test that Python subclasses of native types with attr handlers properly delegate
# attribute storage to the native type's attr handler.
#
# Three code paths are tested:
# 1. SUCCESS: Native attr handler handles the attribute
# 2. FALLBACK: Attr handler doesn't handle it, falls back to dict storage

class MyException(Exception):
    pass

# Test 1: Basic subclass functionality
e1 = MyException("hello", "world")
assert e1.args == ("hello", "world"), "Subclass args failed"
print("Subclass creation: OK")

# Test 2: FALLBACK path - custom attributes fall back to dict storage
e2 = MyException("Test PASS")
e2.custom_attr = "test" # type: ignore
assert e2.custom_attr == "test", "Custom attribute failed"  # type: ignore
print("Dict fallback: OK")
