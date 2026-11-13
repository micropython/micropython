# Port os/random host hooks: entropy seed + urandom, uname (sysname='uefi'), statvfs of
# the boot volume, and getcwd/chdir/listdir over the VFS mounted at "/".
try:
    import os
    import random
except ImportError:
    print("SKIP")
    raise SystemExit

random.seed()
print(0 <= random.getrandbits(30) < (1 << 30))
u = os.urandom(16)
print(isinstance(u, bytes) and len(u) == 16)
print(os.uname().sysname == "uefi")
sv = os.statvfs("/")
print(len(sv) == 10 and sv[0] > 0)
print(os.getcwd() == "/")
os.chdir("/EFI")
print(os.getcwd() == "/EFI")
print("BOOT" in os.listdir())  # listdir() of cwd /EFI
os.chdir("/")
print("os ok")
