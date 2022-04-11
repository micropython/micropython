# Test freezing objects that may not be handled well by the build process.

print("\\")
print("\nX")
print(repr("\x1b"))
print(b"\x00\xff")
