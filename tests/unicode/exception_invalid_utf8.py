# Test that exceptions with strings starting with 0xff don't crash
# Issue #17855

# Test with various sizes of strings starting with compression marker (0xff)
for size in [1, 10, 100, 1000]:
    try:
        # Create a string that starts with 0xff (compression marker)
        s = eval(b"'\\xff" + b"\\xfe" * size + b"'")
        raise ValueError(s)
    except ValueError as e:
        # Just verify we can catch it without crashing
        print(f"Caught ValueError with size {size}")

# Test printing the exception
try:
    raise Exception(eval(b"'\\xff" + b"\\xfe" * 100 + b"'"))
except Exception as e:
    # This used to crash when trying to decompress the "compressed" string
    exc_str = str(e)
    print(f"Exception string starts with: {exc_str[:10]!r}")

print("All tests passed")
