print(b"hello world".index(b"ll"))
print(b"hello world".index(b"ll", None))
print(b"hello world".index(b"ll", 1))
print(b"hello world".index(b"ll", 1, None))
print(b"hello world".index(b"ll", None, None))
print(b"hello world".index(b"ll", 1, -1))
try:
    print(b"hello world".index(b"ll", 1, 1))
except ValueError:
    print("ValueError")
try:
    print(b"hello world".index(b"ll", 1, 2))
except ValueError:
    print("ValueError")
try:
    print(b"hello world".index(b"ll", 1, 3))
except ValueError:
    print("ValueError")
print(b"hello world".index(b"ll", 1, 4))
print(b"hello world".index(b"ll", 1, 5))
print(b"hello world".index(b"ll", -100))
print(b"0000".index(b'0'))
print(b"0000".index(b'0', 0))
print(b"0000".index(b'0', 1))
print(b"0000".index(b'0', 2))
print(b"0000".index(b'0', 3))
try:
    print(b"0000".index(b'0', 4))
except ValueError:
    print("ValueError")
try:
    print(b"0000".index(b'0', 5))
except ValueError:
    print("ValueError")
try:
    print(b"0000".index(b'-1', 3))
except ValueError:
    print("ValueError")
try:
    print(b"0000".index(b'1', 3))
except ValueError:
    print("ValueError")
try:
    print(b"0000".index(b'1', 4))
except ValueError:
    print("ValueError")
try:
    print(b"0000".index(b'1', 5))
except ValueError:
    print("ValueError")
print(b"0000".index(ord(b'0')))
print(b"0000".index(ord(b'0'), 0))
print(b"0000".index(ord(b'0'), 1))
print(b"0000".index(ord(b'0'), 2))
print(b"0000".index(ord(b'0'), 3))
try:
    print(b"0000".index(ord(b'0'), 4))
except ValueError:
    print("ValueError")
try:
    print(b"0000".index(ord(b'0'), 5))
except ValueError:
    print("ValueError")
try:
    print(b"0000".index(ord(b'x'), 3))
except ValueError:
    print("ValueError")
try:
    print(b"0000".index(ord(b'1'), 3))
except ValueError:
    print("ValueError")
try:
    print(b"0000".index(ord(b'1'), 4))
except ValueError:
    print("ValueError")
try:
    print(b"0000".index(ord(b'1'), 5))
except ValueError:
    print("ValueError")

# Non-ascii values (make sure not treated as unicode-like)
print(b"\x80abc".index(b"a", 1))

# Int-like conversion.
print(b"00\x0000".index(b'0', True))

# Out of bounds int.
try:
    print(b"0000".index(b'0', -1))
except ValueError:
    print("ValueError")
try:
    print(b"0000".index(b'0', 256))
except ValueError:
    print("ValueError")
try:
    print(b"0000".index(b'0', 91273611))
except ValueError:
    print("ValueError")
