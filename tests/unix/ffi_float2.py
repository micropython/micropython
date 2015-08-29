# test ffi float support
import sys
try:
    import ffi
except ImportError:
    print("SKIP")
    sys.exit()


def ffi_open(names):
    err = None
    for n in names:
        try:
            mod = ffi.open(n)
            return mod
        except OSError as e:
            err = e
    raise err

libm = ffi_open(('libm.so', 'libm.so.6', 'libc.so.0', 'libc.so.6', 'libc.dylib'))

# Some libc's implement tgammaf as header macro with tgamma(), so don't assume
# it'll be in library.
try:
    tgammaf = libm.func('f', 'tgammaf', 'f')
except OSError:
    print("SKIP")
    sys.exit()

for fun in (tgammaf,):
    for val in (0.5, 1, 1.0, 1.5, 4, 4.0):
        print('%.6f' % fun(val))
