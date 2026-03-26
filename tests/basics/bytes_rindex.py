print(b"hello world".rindex(b"ll"))
print(b"hello world".rindex(b"ll", None))
print(b"hello world".rindex(b"ll", 1))
print(b"hello world".rindex(b"ll", 1, None))
print(b"hello world".rindex(b"ll", None, None))
print(b"hello world".rindex(b"ll", 1, -1))

try:
    print(b"hello world".rindex(b"ll", 1, 1))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

try:
    print(b"hello world".rindex(b"ll", 1, 2))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

try:
    print(b"hello world".rindex(b"ll", 1, 3))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

print(b"hello world".rindex(b"ll", 1, 4))
print(b"hello world".rindex(b"ll", 1, 5))
print(b"hello world".rindex(b"ll", -100))
print(b"0000".rindex(b'0'))
print(b"0000".rindex(b'0', 0))
print(b"0000".rindex(b'0', 1))
print(b"0000".rindex(b'0', 2))
print(b"0000".rindex(b'0', 3))
print(b"0000".rindex(48))
print(b"0000".rindex(48, 0))
print(b"0000".rindex(48, 1))
print(b"0000".rindex(48, 2))
print(b"0000".rindex(48, 3))

try:
    print(b"0000".rindex(b'0', 4))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

try:
    print(b"0000".rindex(48, 4))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

try:
    print(b"0000".rindex(b'0', 5))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

try:
    print(b"0000".rindex(48, 5))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

try:
    print(b"0000".rindex(b'-1', 3))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

try:
    print(b"0000".rindex(b'1', 3))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

try:
    print(b"0000".rindex(49, 3))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

try:
    print(b"0000".rindex(b'1', 4))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

try:
    print(b"0000".rindex(49, 4))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

try:
    print(b"0000".rindex(b'1', 5))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

try:
    print(b"0000".rindex(49, 5))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

