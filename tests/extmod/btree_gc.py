# Test btree interaction with the garbage collector.

try:
    import btree, uio, gc
except ImportError:
    print("SKIP")
    raise SystemExit

N = 80

# Create a BytesIO but don't keep a reference to it.
db = btree.open(uio.BytesIO(), pagesize=512)

# Overwrite lots of the Python stack to make sure no reference to the BytesIO remains.
x = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

# Write lots of key/value pairs, which fill up the DB and also allocate temporary heap
# memory due to the string addition, and do a GC collect to verify that the BytesIO
# is not collected.
for i in range(N):
    db[b"thekey" + str(i)] = b"thelongvalue" + str(i)
    print(db[b"thekey" + str(i)])
    gc.collect()

# Reclaim memory allocated by the db object.
db.close()
