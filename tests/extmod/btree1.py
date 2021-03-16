try:
    import btree
    import uio
    import uerrno
except ImportError:
    print("SKIP")
    raise SystemExit

# f = open("_test.db", "w+b")
f = uio.BytesIO()
db = btree.open(f, pagesize=512)

db[b"foo3"] = b"bar3"
db[b"foo1"] = b"bar1"
db[b"foo2"] = b"bar2"
db[b"bar1"] = b"foo1"

dbstr = str(db)
print(dbstr[:7], dbstr[-1:])

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

print(db.seq(1, b"foo1"))
print(db.seq(1, b"qux"))

try:
    db.seq(b"foo1")
except OSError as e:
    print(e.args[0] == uerrno.EINVAL)

print(list(db.keys()))
print(list(db.values()))

for k in db:
    print(k)

db.put(b"baz1", b"qux1")

print("foo1", "foo1" in db)
print("foo2", "foo2" in db)
print("baz1", "baz1" in db)

try:
    print(db + db[b"foo1"])
except TypeError:
    print("TypeError")

db.flush()
db.close()
f.close()
