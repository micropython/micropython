print(b"".strip())
print(b" \t\n\r\v\f".strip())
print(b" T E S T".strip())
print(b"abcabc".strip(b"ce"))
print(b"aaa".strip(b"b"))
print(b"abc  efg ".strip(b"g a"))

print(b'   spacious   '.lstrip())
print(b'www.example.com'.lstrip(b'cmowz.'))

print(b'   spacious   '.rstrip())
print(b'mississippi'.rstrip(b'ipz'))

# Test that stripping unstrippable string returns original object
s = b"abc"
print(id(s.strip()) == id(s))
