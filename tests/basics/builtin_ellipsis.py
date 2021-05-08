# tests that .../Ellipsis exists

print(...)
print(Ellipsis)

print(... == Ellipsis)

# Test that Ellipsis can be hashed
print(type(hash(Ellipsis)))
