import ffi
import uctypes

libc = ffi.open("libc.so.6")
print("libc:", libc)
print()

# Declare few functions
perror = libc.func("v", "perror", "s")
time = libc.func("i", "time", "p")
open = libc.func("i", "open", "si")
qsort = libc.func("v", "qsort", "piiC")
# And one variable
errno = libc.var("i", "errno")

print("time:", time)
print("UNIX time is:", time(None))
print()

perror("perror before error")
open("somethingnonexistent__", 0)
print("errno object:", errno)
print("errno value:", errno.get())
perror("perror after error")
print()


def cmp(pa, pb):
    a = uctypes.bytearray_at(pa, 1)
    b = uctypes.bytearray_at(pb, 1)
    print("cmp:", a, b)
    return a[0] - b[0]


cmp_cb = ffi.callback("i", cmp, "PP")
print("callback:", cmp_cb)

s = bytearray(b"foobar")
print("org string:", s)
qsort(s, len(s), 1, cmp_cb)
print("qsort'ed string:", s)
