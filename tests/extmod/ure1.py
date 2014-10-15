try:
    import ure as re
except ImportError:
    import re

r = re.compile(".+")
m = r.match("abc")
print(m.group(0))
try:
    m.group(1)
except IndexError:
    print("IndexError")

r = re.compile("(.+)1")
m = r.match("xyz781")
print(m.group(0))
print(m.group(1))
try:
    m.group(2)
except IndexError:
    print("IndexError")

r = re.compile("[a-c]")
m = r.match("a")
print(m.group(0))
m = r.match("d")
print(m)
m = r.match("A")
print(m)

r = re.compile("o+")
m = r.search("foobar")
print(m.group(0))
try:
    m.group(1)
except IndexError:
    print("IndexError")


m = re.match(".*", "foo")
print(m.group(0))

m = re.search("w.r", "hello world")
print(m.group(0))
