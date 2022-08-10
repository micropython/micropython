# test bytearray with its re-use of byte functions

print(bytearray(b"hello world").find(b"ll"))
print(bytearray(b"hello\x00world").rfind(b"l"))

print(bytearray(b"abc  efg ").strip(b"g a"))
print(bytearray(b"   spacious   ").lstrip())
print(bytearray(b"www.example.com").lstrip(b"cmowz."))
print(bytearray(b"   spacious   ").rstrip())
print(bytearray(b"mississippi").rstrip(b"ipz"))

print(bytearray(b"abc").split(b"a"))
print(bytearray(b"abcabc").rsplit(b"bc"))

print(bytearray(b"asdfasdf").replace(b"a", b"b"))

print("00\x0000".index("0", 0))
print("00\x0000".index("0", 3))
print("00\x0000".rindex("0", 0))
print("00\x0000".rindex("0", 3))

print(bytearray(b"foobar").endswith(b"bar"))
print(bytearray(b"1foo").startswith(b"foo", 1))

print(bytearray(b" T E \x00 S T").lower())
print(bytearray(b" te \x00 st").upper())

print(bytearray(b" \t\n\r\v\f").isspace())
print(bytearray(b"this ").isalpha())
print(bytearray(b"0123456789").isdigit())
print(bytearray(b"AB").isupper())
print(bytearray(b"cheese-cake").islower())

print(bytearray(b",").join((bytearray(b"abc"), bytearray(b"def"))))
print(type(bytearray(b",").join((b"a", b"b", b"c"))))
