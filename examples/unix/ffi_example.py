import ffi

libc = ffi.open("libc.so.6")
print("libc:", libc)
print()

# Declare few functions
perror = libc.func("v", "perror", "s")
time = libc.func("i", "time", "p")
open = libc.func("i", "open", "si")
qsort = libc.func("v", "qsort", "piip")
# And one variable
errno = libc.var("i", "errno")

print("time:", time)
print("UNIX time is:", time(None))
print()

perror("ffi before error")
open("somethingnonexistent__", 0)
print("errno object:", errno)
print("errno value:", errno.get())
perror("ffi after error")
print()

def cmp(pa, pb):
    a = ffi.as_bytearray(pa, 1)
    b = ffi.as_bytearray(pb, 1)
    print("cmp:", a, b)
    return a[0] - b[0]

cmp_c = ffi.callback("i", cmp, "pp")
print("callback:", cmp_c)

s = bytearray(b"foobar")
print("org string:", s)
qsort(s, len(s), 1, cmp_c)
print("qsort'ed string:", s)
