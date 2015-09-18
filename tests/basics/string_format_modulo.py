print("%%" % ())
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
print("%.1s" % "ab")

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

print("%*d" % (5, 10))
print("%*.*d" % (2, 2, 20))
print("%*.*d" % (5, 8, 20))

print(">%8.4d<" % -12)
print(">% 8.4d<" % -12)
print(">%+8.4d<" %  12)
print(">%+8.4d<" % -12)
print(">%08.4d<" % -12)
print(">%08.4d<" % 12)
print(">%-8.4d<" % -12)
print(">%-08.4d<" % -12)
print(">%-+08.4d<" % -12)
print(">%-+08.4d<" %  12)

# Cases when "*" used and there's not enough values total
try:
    print("%*s" % 5)
except TypeError:
    print("TypeError")
try:
    print("%*.*s" % (1, 15))
except TypeError:
    print("TypeError")

print("%(foo)s" % {"foo": "bar", "baz": False})
try:
    print("%(foo)s" % {})
except KeyError:
    print("KeyError")
# Using in "*" with dict got to fail
try:
    print("%(foo)*s" % {"foo": "bar"})
except TypeError:
    print("TypeError")

try:
    '%(a' % {'a':1}
except ValueError:
    print('ValueError')

try:
    '%.*d %.*d' % (20, 5)
except TypeError:
    print('TypeError')

try:
    a = '%*' % 1
except (ValueError):
    print('ValueError')

try:
    '%c' % 'aa'
except TypeError:
    print('TypeError')

try:
    '%l' % 1
except ValueError:
    print('ValueError')

try:
    'a%' % 1
except ValueError:
    print('ValueError')
