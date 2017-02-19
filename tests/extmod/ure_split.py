try:
    import ure as re
except ImportError:
    try:
        import re
    except ImportError:
        import sys
        print("SKIP")
        sys.exit()

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
