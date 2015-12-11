import sys
try:
    import uos as os
except ImportError:
    import os

if not hasattr(os, "unlink"):
    print("SKIP")
    sys.exit()

# cleanup in case testfile exists
try:
    os.unlink("testfile")
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
    os.unlink("testfile")
except OSError:
    pass
