try:
    str.partition
except AttributeError:
    print("SKIP")
    raise SystemExit

print("asdf".partition('g'))
print("asdf".partition('a'))
print("asdf".partition('s'))
print("asdf".partition('f'))
print("asdf".partition('d'))
print("asdf".partition('asd'))
print("asdf".partition('sdf'))
print("asdf".partition('as'))
print("asdf".partition('df'))
print("asdf".partition('asdf'))
print("asdf".partition('asdfa'))
print("asdf".partition('fasdf'))
print("asdf".partition('fasdfa'))
print("abba".partition('a'))
print("abba".partition('b'))

try:
    print("asdf".partition(1))
except TypeError:
    print("Raised TypeError")
else:
    print("Did not raise TypeError")

try:
    print("asdf".partition(''))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")

# Bytes
print(b"abba".partition(b'b'))
try:
    print(b"abba".partition('b'))
except TypeError:
    print("Raised TypeError")
try:
    print("abba".partition(b'b'))
except TypeError:
    print("Raised TypeError")
