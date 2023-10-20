try:
    import re
except ImportError:
    print("SKIP")
    raise SystemExit

r = re.compile(" ")
s = r.split("a b c foobar")
print(s)

r = re.compile(" +")
s = r.split("a b    c   foobar")
print(s)

r = re.compile(" +")
s = r.split("a b    c   foobar", 1)
print(s)

r = re.compile(" +")
s = r.split("a b    c   foobar", 2)
print(s)

r = re.compile("[a-f]+")
s = r.split("0a3b9")
print(s)

# bytes objects
r = re.compile(b"x")
s = r.split(b"fooxbar")
print(s)

# using ^
r = re.compile("^ab")
s = r.split("abababcabab")
print(s)

# using ^ with |
r = re.compile("^ab|cab")
s = r.split("abababcabab")
print(s)
