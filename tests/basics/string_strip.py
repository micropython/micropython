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
