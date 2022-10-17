# Test for VfsPosix

try:
    import uos
    import utime

    uos.VfsPosix
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

# We need a directory for testing that doesn't already exist.
# Skip the test if it does exist.
temp_dir = "micropy_test_dir"
try:
    uos.stat(temp_dir)
    print("SKIP")
    raise SystemExit
except OSError:
    pass

# mkdir
uos.mkdir(temp_dir)

test1 = temp_dir + "/test1"

# Create an empty file, should have a timestamp.
open(test1, "wt").close()

# Stat the file before and after setting mtime with os.utime().
stat1 = uos.stat(test1)
tm = utime.mktime((2010, 1, 1, 11, 49, 1, 0, 0))
uos.utime(test1, (tm, tm))
stat2 = uos.stat(test1)
print(stat1[8] != 0, stat2[8] != stat1[8], stat2[8] == tm)

# Check that uos.utime(f) and uos.utime(f, None) set mtime to current time
current_time = utime.time()
uos.utime(test1)
stat3 = uos.stat(test1)
print(stat3[8] - current_time < 100)
uos.utime(test1, (0, 0))
uos.utime(test1, None)
stat4 = uos.stat(test1)
print(stat4[8] - current_time < 100)

# Check for invalid arguments: should raise TypeError.
try:
    uos.utime(test1, (None, None))
except TypeError:
    print("TypeError")
try:
    uos.utime(test1, (None, "Invalid"))
except TypeError:
    print("TypeError")

uos.remove(test1)
uos.rmdir(temp_dir)
