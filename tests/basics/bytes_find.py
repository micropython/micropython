print(b"hello world".find(b"ll"))
print(b"hello world".find(b"ll", None))
print(b"hello world".find(b"ll", 1))
print(b"hello world".find(b"ll", 1, None))
print(b"hello world".find(b"ll", None, None))
print(b"hello world".find(b"ll", 1, -1))
print(b"hello world".find(b"ll", 1, 1))
print(b"hello world".find(b"ll", 1, 2))
print(b"hello world".find(b"ll", 1, 3))
print(b"hello world".find(b"ll", 1, 4))
print(b"hello world".find(b"ll", 1, 5))
print(b"hello world".find(b"ll", -100))
print(b"0000".find(b'0'))
print(b"0000".find(b'0', 0))
print(b"0000".find(b'0', 1))
print(b"0000".find(b'0', 2))
print(b"0000".find(b'0', 3))
print(b"0000".find(b'0', 4))
print(b"0000".find(b'0', 5))
print(b"0000".find(b'-1', 3))
print(b"0000".find(b'1', 3))
print(b"0000".find(b'1', 4))
print(b"0000".find(b'1', 5))
print(b"0000".find(ord(b'0')))
print(b"0000".find(ord(b'0'), 0))
print(b"0000".find(ord(b'0'), 1))
print(b"0000".find(ord(b'0'), 2))
print(b"0000".find(ord(b'0'), 3))
print(b"0000".find(ord(b'0'), 4))
print(b"0000".find(ord(b'0'), 5))
print(b"0000".find(ord(b'x'), 3))
print(b"0000".find(ord(b'1'), 3))
print(b"0000".find(ord(b'1'), 4))
print(b"0000".find(ord(b'1'), 5))

# Non-ascii values (make sure not treated as unicode-like)
print(b"\x80abc".find(b"a", 1))

# Int-like conversion.
print(b"00\x0000".find(b'0', True))

# Out of bounds int.
try:
    print(b"0000".find(b'0', -1))
except ValueError:
    print("ValueError")
try:
    print(b"0000".find(b'0', 256))
except ValueError:
    print("ValueError")
try:
    print(b"0000".find(b'0', 91273611))
except ValueError:
    print("ValueError")
