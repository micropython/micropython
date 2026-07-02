# Test str.center() with Unicode characters
# Issue #17827

try:
    str.center("a", 1)
except (AttributeError, NameError):
    print("SKIP")
    raise SystemExit

# ASCII baseline
print("hello".center(10))

# Latin with accent (é is 2 bytes in UTF-8)
print("héllo".center(10))

# Chinese (each char is 3 bytes in UTF-8)
print("你好".center(10))

# Emoji (4 bytes in UTF-8)
print("🎉".center(5))

# German with umlaut
print("München".center(15))

# Cyrillic
print("Москва".center(12))

# Edge cases
print("test".center(4))  # Exact fit
print("test".center(3))  # String longer than width
print("x".center(1))  # Single char, exact fit
print("".center(5))  # Empty string

# Mixed ASCII and Unicode
print("café".center(10))
print("hello世界".center(12))
