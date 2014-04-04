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

print("%c" % 48)
print("%c" % 'a')
print("%10s" % 'abc')
print("%-10s" % 'abc')
print("%d" % 10)
print("%+d" % 10)
print("% d" % 10)
print("%d" % -10)
print("%x" % 18)
print("%o" % 18)
print("%X" % 18)
print("%#x" % 18)
print("%#X" % 18)
print("%#6x" % 18)
print("%#06x" % 18)
print("%e" % 1.23456)
print("%E" % 1.23456)
print("%f" % 1.23456)
print("%F" % 1.23456)
print("%g" % 1.23456)
print("%G" % 1.23456)
