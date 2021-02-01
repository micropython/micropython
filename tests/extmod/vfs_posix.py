# Test for VfsPosix

try:
    import uos

    uos.VfsPosix
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


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
