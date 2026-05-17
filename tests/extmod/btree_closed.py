try:
    import btree
    import io
except ImportError:
    print("SKIP")
    raise SystemExit

f = io.BytesIO()
db = btree.open(f)

db[b"foo"] = b"42"

db.close()

# Accessing an already-closed database should fail.
try:
    print(db[b"foo"])
except ValueError:
    print("ValueError")

try:
    db[b"bar"] = b"43"
except ValueError:
    print("ValueError")

try:
    db.flush()
except ValueError:
    print("ValueError")

try:
    for k, v in db.items():
        pass
except ValueError:
    print("ValueError")

# Closing and printing an already-closed database should not fail.
db.close()
print(db)
