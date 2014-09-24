# test ffi float support

import ffi

def ffi_open(names):
    err = None
    for n in names:
        try:
            mod = ffi.open(n)
            return mod
        except OSError as e:
            err = e
    raise err

libc = ffi_open(('libc.so', 'libc.so.0', 'libc.so.6', 'libc.dylib'))

strtof = libc.func("f", "strtof", "sp")
print('%.6f' % strtof('1.23', None))

strtod = libc.func("d", "strtod", "sp")
print('%.6f' % strtod('1.23', None))
