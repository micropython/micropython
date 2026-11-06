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


libc = ffi_open(("libc.so", "libc.so.0", "libc.so.6", "libc.so.7", "libc.dylib"))

# Calling an FFI function with the wrong number of arguments must raise
# TypeError rather than crash the interpreter (GH-19832).
abs_fn = libc.func("i", "abs", "i")

try:
    abs_fn()
except TypeError:
    print("TypeError")

try:
    abs_fn(1, 2)
except TypeError:
    print("TypeError")

# Keyword arguments are rejected even with the correct positional count.
try:
    abs_fn(-5, unexpected=1)
except TypeError:
    print("TypeError")

# the correct number of arguments still works
print(abs_fn(-5))
