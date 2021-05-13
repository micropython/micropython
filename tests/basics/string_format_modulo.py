try:
    '' % ()
except TypeError:
    print("SKIP")
    raise SystemExit

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

print('%c' % False)
print('%c' % True)

# Should be able to print dicts; in this case they aren't used
# to lookup keywords in formats like %(foo)s
print('%s' % {})
print('%s' % ({},))

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
print("%s %(foo)s %(foo)s" % {"foo": 1})
try:
    print("%(foo)s" % {})
except KeyError:
    print("KeyError")
# Using in "*" with dict got to fail
try:
    print("%(foo)*s" % {"foo": "bar"})
except TypeError:
    print("TypeError")

# When using %(foo)s format the single argument must be a dict
try:
    '%(foo)s' % 1
except TypeError:
    print('TypeError')
try:
    '%(foo)s' % ({},)
except TypeError:
    print('TypeError')

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
