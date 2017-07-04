# test splitting with pattern matches that can be empty
#
# CPython 3.5 issues a FutureWarning for these tests because their
# behaviour will change in a future version.  MicroPython just stops
# splitting as soon as an empty match is found.

try:
    import ure as re
except ImportError:
    print("SKIP")
    raise SystemExit

r = re.compile(" *")
s = r.split("a b    c   foobar")
print(s)

r = re.compile("x*")
s = r.split("foo")
print(s)

r = re.compile("x*")
s = r.split("axbc")
print(s)
