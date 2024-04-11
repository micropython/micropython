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

qsort = libc.func("v", "qsort", "piip")


def cmp(pa, pb):
    a = ffi.as_bytearray(pa, 1)
    b = ffi.as_bytearray(pb, 1)
    # print("cmp:", a, b)
    return a[0] - b[0]


cmp_c = ffi.callback("i", cmp, "pp")

s = bytearray(b"foobar")
print("org string:", s)
qsort(s, len(s), 1, cmp_c)
print("qsort'ed:", s)
