try:
    import ure as re
except ImportError:
    try:
        import re
    except ImportError:
        print("SKIP")
        raise SystemExit

r = re.compile(".+")
m = r.match("abc")
print(m.group(0))
try:
    m.group(1)
except IndexError:
    print("IndexError")

# conversion of re and match to string
str(r)
str(m)

r = re.compile("(.+)1")
m = r.match("xyz781")
print(m.group(0))
print(m.group(1))
try:
    m.group(2)
except IndexError:
    print("IndexError")

r = re.compile("[a-cu-z]")
m = r.match("a")
print(m.group(0))
m = r.match("z")
print(m.group(0))
m = r.match("d")
print(m)
m = r.match("A")
print(m)
print("===")

r = re.compile("[^a-cu-z]")
m = r.match("a")
print(m)
m = r.match("z")
print(m)
m = r.match("d")
print(m.group(0))
m = r.match("A")
print(m.group(0))


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

m = re.match('a+?', 'ab');  print(m.group(0))
m = re.match('a*?', 'ab');  print(m.group(0))
m = re.match('^ab$', 'ab'); print(m.group(0))
m = re.match('a|b', 'b');   print(m.group(0))
m = re.match('a|b|c', 'c'); print(m.group(0))

# Case where anchors fail to match
r = re.compile("^b|b$")
m = r.search("abc")
print(m)

try:
    re.compile("*")
except:
    print("Caught invalid regex")

# bytes objects
m = re.match(rb'a+?', b'ab');  print(m.group(0))
