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
print("===")

# '-' character within character class block
print(re.match("[-a]+", "-a]d").group(0))
print(re.match("[a-]+", "-a]d").group(0))
print("===")

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
print("===")

# escaping
m = re.match(r'a\.c', 'a.c'); print(m.group(0) if m else '')
m = re.match(r'a\.b', 'abc'); print(m is None)
m = re.match(r'a\.b', 'a\\bc'); print(m is None)
m = re.match(r'[a\-z]', 'abc'); print(m.group(0))
m = re.match(r'[.\]]*', '.].]a'); print(m.group(0))
m = re.match(r'[.\]+]*', '.]+.]a'); print(m.group(0))
m = re.match(r'[a-f0-9x\-yz]*', 'abxcd1-23'); print(m.group(0))
m = re.match(r'[a\\b]*', 'a\\aa\\bb\\bbab'); print(m.group(0))
m = re.search(r'[a\-z]', '-'); print(m.group(0))
m = re.search(r'[a\-z]', 'f'); print(m is None)
m = re.search(r'[a\]z]', 'a'); print(m.group(0))
print(re.compile(r'[-a]').split('foo-bar'))
print(re.compile(r'[a-]').split('foo-bar'))
print(re.compile(r'[ax\-]').split('foo-bar'))
print(re.compile(r'[a\-x]').split('foo-bar'))
print(re.compile(r'[\-ax]').split('foo-bar'))
print("===")
