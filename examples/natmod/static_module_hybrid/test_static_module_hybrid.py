"""Test script for the hybrid static native module."""

import static_module_hybrid

print("Testing hybrid static native module...")

# Test the double function
result = static_module_hybrid.double(21)
print(f"double(21) = {result}")
assert result == 42

# Test the add function
result = static_module_hybrid.add(10, 32)
print(f"add(10, 32) = {result}")
assert result == 42

# Test the message function
msg = static_module_hybrid.message()
print(f"message() = {msg}")
assert msg == "Hello_from_hybrid_module"

# Test the constant
print(f"MAGIC_NUMBER = {static_module_hybrid.MAGIC_NUMBER}")
assert static_module_hybrid.MAGIC_NUMBER == 42

print("All tests passed!")
