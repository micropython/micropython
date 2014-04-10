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
print("%s" % 1.0)

print("%r" % True)
print("%r" % 1)
print("%r" % 1.0)

print("%c" % 48)
print("%c" % 'a')
print("%10s" % 'abc')
print("%-10s" % 'abc')
print("%d" % 10)
print("%+d" % 10)
print("% d" % 10)
print("%d" % -10)
print("%d" % 1.0)
print("%d" % True)
print("%i" % -10)
print("%i" % 1.0)
print("%i" % True)
print("%u" % -10)
print("%u" % 1.0)
print("%u" % True)
print("%x" % 18)
print("%x" % 18.0)
print("%o" % 18)
print("%o" % 18.0)
print("%X" % 18)
print("%X" % 18.0)
print("%#x" % 18)
print("%#X" % 18)
print("%#6o" % 18)
print("%#6x" % 18)
print("%#06x" % 18)
print("%e" % 1.23456)
print("%E" % 1.23456)
print("%f" % 1.23456)
print("%F" % 1.23456)
print("%g" % 1.23456)
print("%G" % 1.23456)
