# Test for VfsPosix

try:
    import uos

    uos.VfsPosix
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

# We need a file for testing that doesn't already exist.
# Skip the test if it does exist.
temp_file = "micropy_test_file.txt"
try:
    uos.stat(temp_file)
    print("SKIP")
    raise SystemExit
except OSError:
    pass

# getcwd and chdir
curdir = uos.getcwd()
uos.chdir("/")
print(uos.getcwd())
uos.chdir(curdir)
print(uos.getcwd() == curdir)

# stat
print(type(uos.stat("/")))

# listdir and ilistdir
print(type(uos.listdir("/")))

# file create
f = open(temp_file, "w")
f.write("hello")
f.close()

# close on a closed file should succeed
f.close()

# file read
f = open(temp_file, "r")
print(f.read())
f.close()

# remove
uos.remove(temp_file)
