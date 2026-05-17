print("".strip())
print(" \t\n\r\v\f".strip())
print(" T E S T".strip())
print("abcabc".strip("ce"))
print("aaa".strip("b"))
print("abc  efg ".strip("g a"))

print('   spacious   '.lstrip())
print('www.example.com'.lstrip('cmowz.'))

print('   spacious   '.rstrip())
print('mississippi'.rstrip('ipz'))

print(b'mississippi'.rstrip(b'ipz'))
try:
    print(b'mississippi'.rstrip('ipz'))
except TypeError:
    print("TypeError")
try:
    print('mississippi'.rstrip(b'ipz'))
except TypeError:
    print("TypeError")

# single-char subj string used to give a problem
print("a".strip())
print("a".lstrip())
print("a".rstrip())
print(" a".strip())
print(" a".lstrip())
print(" a".rstrip())
print("a ".strip())
print("a ".lstrip())
print("a ".rstrip())

# \0 used to give a problem

print("\0abc\0".strip())
print("\0abc\0".lstrip())
print("\0abc\0".rstrip())
print("\0abc\0".strip("\0"))

# Test that stripping unstrippable string returns original object
s = "abc"
print(id(s.strip()) == id(s))
