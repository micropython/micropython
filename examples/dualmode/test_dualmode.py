"""Test script for dual-mode module."""

import dualmode_example

print("Testing dualmode module...")

# Test add function
result = dualmode_example.add(10, 20)
print(f"add(10, 20) = {result}")
assert result == 30

# Test hello function
msg = dualmode_example.hello()
print(f"hello() = {msg}")
assert msg == "Hello_from_dualmode"

# Test double function
result = dualmode_example.double(21)
print(f"double(21) = {result}")
assert result == 42

# Test VERSION constant
print(f"VERSION = {dualmode_example.VERSION}")
assert dualmode_example.VERSION == 100

print("All tests passed!")