# test ffi float support
try:
    import ffi
except ImportError:
    print("SKIP")
    raise SystemExit


def ffi_open(names):
    err = None
    for n in names:
        try:
            mod = ffi.open(n)
            return mod
        except OSError as e:
            err = e
    raise err


libc = ffi_open(("libc.so", "libc.so.0", "libc.so.6", "libc.dylib"))

try:
    strtof = libc.func("f", "strtof", "sp")
except OSError:
    # Some libc's (e.g. Android's Bionic) define strtof as macro/inline func
    # in terms of strtod().
    print("SKIP")
    raise SystemExit

print("%.6f" % strtof("1.23", None))

strtod = libc.func("d", "strtod", "sp")
print("%.6f" % strtod("1.23", None))

# test passing double and float args
libm = ffi_open(("libm.so", "libm.so.6", "libc.so.0", "libc.so.6", "libc.dylib"))
tgamma = libm.func("d", "tgamma", "d")
for fun in (tgamma,):
    for val in (0.5, 1, 1.0, 1.5, 4, 4.0):
        print("%.6f" % fun(val))
