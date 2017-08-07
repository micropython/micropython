try:
    str.partition
except AttributeError:
    print("SKIP")
    raise SystemExit

print("asdf".rpartition('g'))
print("asdf".rpartition('a'))
print("asdf".rpartition('s'))
print("asdf".rpartition('f'))
print("asdf".rpartition('d'))
print("asdf".rpartition('asd'))
print("asdf".rpartition('sdf'))
print("asdf".rpartition('as'))
print("asdf".rpartition('df'))
print("asdf".rpartition('asdf'))
print("asdf".rpartition('asdfa'))
print("asdf".rpartition('fasdf'))
print("asdf".rpartition('fasdfa'))
print("abba".rpartition('a'))
print("abba".rpartition('b'))

try:
    print("asdf".rpartition(1))
except TypeError:
    print("Raised TypeError")
else:
    print("Did not raise TypeError")

try:
    print("asdf".rpartition(''))
except ValueError:
    print("Raised ValueError")
else:
    print("Did not raise ValueError")
