"""Test script for the traditional static native module."""

import static_module_traditional

print("Testing traditional static native module...")

# Test the double function
result = static_module_traditional.double(21)
print(f"double(21) = {result}")
assert result == 42

# Test the add function
result = static_module_traditional.add(10, 32)
print(f"add(10, 32) = {result}")
assert result == 42

# Test the message function
msg = static_module_traditional.message()
print(f"message() = {msg}")
assert msg == "Hello_from_traditional_module"

# Test the constant
print(f"MAGIC_NUMBER = {static_module_traditional.MAGIC_NUMBER}")
assert static_module_traditional.MAGIC_NUMBER == 42

print("All tests passed!")