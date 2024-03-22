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
for fun_name in ("tgamma",):
    fun = globals()[fun_name]
    for val in (0.5, 1, 1.0, 1.5, 4, 4.0):
        print(fun_name, "%.5f" % fun(val))

# test passing 2x float/double args
powf = libm.func("f", "powf", "ff")
pow = libm.func("d", "pow", "dd")
for fun_name in ("powf", "pow"):
    fun = globals()[fun_name]
    for args in ((0, 1), (1, 0), (2, 0.5), (3, 4)):
        print(fun_name, "%.5f" % fun(*args))
