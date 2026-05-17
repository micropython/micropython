# default separator (whitespace)
print("a b".split())
print("   a   b    ".split(None))
print("   a   b    ".split(None, 1))
print("   a   b    ".split(None, 2))
print("   a   b  c  ".split(None, 1))
print("   a   b  c  ".split(None, 0))
print("   a   b  c  ".split(None, -1))
print("foo\n\t\x07\v\nbar".split())
print("foo\nbar\n".split())

# empty separator should fail
try:
    "abc".split('')
except ValueError:
    print("ValueError")

# non-empty separator
print("abc".split("a"))
print("abc".split("b"))
print("abc".split("c"))
print("abc".split("z"))
print("abc".split("ab"))
print("abc".split("bc"))
print("abc".split("abc"))
print("abc".split("abcd"))
print("abcabc".split("bc"))
print("abcabc".split("bc", 0))
print("abcabc".split("bc", 1))
print("abcabc".split("bc", 2))

print(b"abcabc".split(b"bc", 2))
