# default separator (whitespace)
print("a b".rsplit())
#print("   a   b    ".rsplit(None))
#print("   a   b    ".rsplit(None, 1))
#print("   a   b    ".rsplit(None, 2))
#print("   a   b  c  ".rsplit(None, 1))
#print("   a   b  c  ".rsplit(None, 0))
#print("   a   b  c  ".rsplit(None, -1))

# empty separator should fail
try:
    "abc".rsplit('')
except ValueError:
    print("ValueError")

# non-empty separator
print("abc".rsplit("a"))
print("abc".rsplit("b"))
print("abc".rsplit("c"))
print("abc".rsplit("z"))
print("abc".rsplit("ab"))
print("abc".rsplit("bc"))
print("abc".rsplit("abc"))
print("abc".rsplit("abcd"))
print("abcabc".rsplit("bc"))
print("abcabc".rsplit("bc", 0))
print("abcabc".rsplit("bc", 1))
print("abcabc".rsplit("bc", 2))

print("10/11/12".rsplit("/", 1))
print("10/11/12".rsplit("/", 2))
print("10/11/12".rsplit("/", 3))
print("10/11/12".rsplit("/", 4))
print("10/11/12".rsplit("/", 5))

print("/*10/*11/*12/*".rsplit("/*", 1))
print("/*10/*11/*12/*".rsplit("/*", 2))
print("/*10/*11/*12/*".rsplit("/*", 3))
print("/*10/*11/*12/*".rsplit("/*", 4))
print("/*10/*11/*12/*".rsplit("/*", 5))

print(b"abcabc".rsplit(b"bc", 2))
