try:
    import uos as os
except ImportError:
    import os

if not hasattr(os, "remove"):
    print("SKIP")
    raise SystemExit

# cleanup in case testfile exists
try:
    os.remove("testfile")
except OSError:
    pass

# Should create a file
f = open("testfile", "a")
f.write("foo")
f.close()

f = open("testfile")
print(f.read())
f.close()

f = open("testfile", "a")
f.write("bar")
f.close()

f = open("testfile")
print(f.read())
f.close()

# cleanup
try:
    os.remove("testfile")
except OSError:
    pass
