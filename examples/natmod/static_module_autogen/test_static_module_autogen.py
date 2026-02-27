"""Test script for the auto-generated static native module."""

import static_module_autogen

print("Testing auto-generated static native module...")

# Test the double function
result = static_module_autogen.double(21)
print(f"double(21) = {result}")
assert result == 42

# Test the add function
result = static_module_autogen.add(10, 32)
print(f"add(10, 32) = {result}")
assert result == 42

# Test the message function
msg = static_module_autogen.message()
print(f"message() = {msg}")
assert msg == "Hello_from_autogen_module"

# Test the constant
print(f"MAGIC_NUMBER = {static_module_autogen.MAGIC_NUMBER}")
assert static_module_autogen.MAGIC_NUMBER == 42

print("All tests passed!")