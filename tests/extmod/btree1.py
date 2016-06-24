try:
    import btree
except ImportError:
    print("SKIP")
    import sys
    sys.exit()

db = btree.open(None)
db[b"foo3"] = b"bar3"
db[b"foo1"] = b"bar1"
db[b"foo2"] = b"bar2"
db[b"bar1"] = b"foo1"

print(db[b"foo2"])
try:
    print(db[b"foo"])
except KeyError:
    print("KeyError")
print(db.get(b"foo"))
print(db.get(b"foo", b"dflt"))

del db[b"foo2"]
try:
    del db[b"foo"]
except KeyError:
    print("KeyError")

for k, v in db.items():
    print((k, v))

print("---")
for k, v in db.items(None, None):
    print((k, v))

print("---")
for k, v in db.items(b"f"):
    print((k, v))

print("---")
for k, v in db.items(b"f", b"foo3"):
    print((k, v))

print("---")
for k, v in db.items(None, b"foo3"):
    print((k, v))

print("---")
for k, v in db.items(b"f", b"foo3", btree.INCL):
    print((k, v))

print("---")
for k, v in db.items(None, None, btree.DESC):
    print((k, v))

print(list(db.keys()))
print(list(db.values()))

for k in db:
    print(k)
