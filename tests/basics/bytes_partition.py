try:
    str.partition
except AttributeError:
    print("SKIP")
    raise SystemExit

print(b"asdf".partition(b'g'))
print(b"asdf".partition(b'a'))
print(b"asdf".partition(b's'))
print(b"asdf".partition(b'f'))
print(b"asdf".partition(b'd'))
print(b"asdf".partition(b'asd'))
print(b"asdf".partition(b'sdf'))
print(b"asdf".partition(b'as'))
print(b"asdf".partition(b'df'))
print(b"asdf".partition(b'asdf'))
print(b"asdf".partition(b'asdfa'))
print(b"asdf".partition(b'fasdf'))
print(b"asdf".partition(b'fasdfa'))
print(b"abba".partition(b'a'))
print(b"abba".partition(b'b'))

try:
    print(b"asdf".partition(1))
except TypeError:
    print("Raised TypeError")
else:
    print("Did not raise TypeError")

try:
    print(b"asdf".partition(b''))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")
