print("=%s=" % 1)
print("=%s=%s=" % (1, 2))
print("=%s=" % (1,))
print("=%s=" % [1, 2])

print("=%s=" % "str")
print("=%r=" % "str")

try:
    print("=%s=%s=" % 1)
except TypeError:
    print("TypeError")

try:
    print("=%s=%s=%s=" % (1, 2))
except TypeError:
    print("TypeError")

try:
    print("=%s=" % (1, 2))
except TypeError:
    print("TypeError")

print("%s" % True)
print("%s" % 1)

print("%r" % True)
print("%r" % 1)

print("%c" % 48)
print("%c" % 'a')
print("%10s" % 'abc')
print("%-10s" % 'abc')
print("%d" % 10)
print("%+d" % 10)
print("% d" % 10)
print("%d" % -10)
print("%d" % True)
print("%i" % -10)
print("%i" % True)
print("%u" % -10)
print("%u" % True)
print("%x" % 18)
print("%o" % 18)
print("%X" % 18)
print("%#x" % 18)
print("%#X" % 18)
print("%#6o" % 18)
print("%#6x" % 18)
print("%#06x" % 18)
