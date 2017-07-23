try:
    import uos as os
except ImportError:
    import os

if not hasattr(os, "unlink"):
    print("SKIP")
    raise SystemExit

# cleanup in case testfile exists
try:
    os.unlink("testfile")
except OSError:
    pass

try:
    f = open("testfile", "r+b")
    print("Unexpectedly opened non-existing file")
except OSError:
    print("Expected OSError")
    pass

f = open("testfile", "w+b")
f.write(b"1234567890")
f.seek(0)
print(f.read())
f.close()

# Open with truncation
f = open("testfile", "w+b")
f.write(b"abcdefg")
f.seek(0)
print(f.read())
f.close()

# Open without truncation
f = open("testfile", "r+b")
f.write(b"1234")
f.seek(0)
print(f.read())
f.close()

# cleanup
try:
    os.unlink("testfile")
except OSError:
    pass
