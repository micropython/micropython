# default separator (whitespace)
print(b"a b".split())
print(b"   a   b    ".split(None))
print(b"   a   b    ".split(None, 1))
print(b"   a   b    ".split(None, 2))
print(b"   a   b  c  ".split(None, 1))
print(b"   a   b  c  ".split(None, 0))
print(b"   a   b  c  ".split(None, -1))

# empty separator should fail
try:
    b"abc".split(b'')
except ValueError:
    print("ValueError")

# non-empty separator
print(b"abc".split(b"a"))
print(b"abc".split(b"b"))
print(b"abc".split(b"c"))
print(b"abc".split(b"z"))
print(b"abc".split(b"ab"))
print(b"abc".split(b"bc"))
print(b"abc".split(b"abc"))
print(b"abc".split(b"abcd"))
print(b"abcabc".split(b"bc"))
print(b"abcabc".split(b"bc", 0))
print(b"abcabc".split(b"bc", 1))
print(b"abcabc".split(b"bc", 2))
