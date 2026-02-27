"""Test script for the static native module."""

import static_module

print("Testing static native module...")

# Test the double function
result = static_module.double(21)
print(f"double(21) = {result}")
assert result == 42

# Test the add function
result = static_module.add(10, 32)
print(f"add(10, 32) = {result}")
assert result == 42

# Test the message function
msg = static_module.message()
print(f"message() = {msg}")
assert msg == "Hello_from_static_module"

# Test the constant
print(f"MAGIC_NUMBER = {static_module.MAGIC_NUMBER}")
assert static_module.MAGIC_NUMBER == 42

print("All tests passed!")