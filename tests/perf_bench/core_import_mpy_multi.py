# Test performance of importing an .mpy file many times.

import usys, uio, uos

if not (hasattr(uio, "IOBase") and hasattr(uos, "mount")):
    print("SKIP")
    raise SystemExit

# This is the test.py file that is compiled to test.mpy below.
"""
class A:
    def __init__(self, arg):
        self.arg = arg
    def write(self):
        pass
    def read(self):
        pass
def f():
    print, str, bytes, dict
    Exception, ValueError, TypeError
    x = "this will be a string object"
    x = b"this will be a bytes object"
    x = ("const tuple", None, False, True, 1, 2, 3)
result = 123
"""
file_data = b'M\x06\x02\x1f\x14\x03\x0etest.py\x00\x0f\x02A\x00\x02f\x00\x0cresult\x00/-5#\x82I\x81{\x81w\x82/\x81\x05\x81\x17Iom\x82\x13\x06arg\x00\x05\x1cthis will be a string object\x00\x06\x1bthis will be a bytes object\x00\n\x07\x05\x0bconst tuple\x00\x01\x02\x03\x07\x011\x07\x012\x07\x013\x81\\\x10\n\x01\x89\x07d`T2\x00\x10\x024\x02\x16\x022\x01\x16\x03"\x80{\x16\x04Qc\x02\x81d\x00\x08\x02(DD\x11\x05\x16\x06\x10\x02\x16\x072\x00\x16\x082\x01\x16\t2\x02\x16\nQc\x03`\x1a\x08\x08\x12\x13@\xb1\xb0\x18\x13Qc@\t\x08\t\x12` Qc@\t\x08\n\x12``Qc\x82@ \x0e\x03\x80\x08+)##\x12\x0b\x12\x0c\x12\r\x12\x0e*\x04Y\x12\x0f\x12\x10\x12\x11*\x03Y#\x00\xc0#\x01\xc0#\x02\xc0Qc'


class File(uio.IOBase):
    def __init__(self):
        self.off = 0

    def ioctl(self, request, arg):
        return 0

    def readinto(self, buf):
        buf[:] = memoryview(file_data)[self.off : self.off + len(buf)]
        self.off += len(buf)
        return len(buf)


class FS:
    def mount(self, readonly, mkfs):
        pass

    def chdir(self, path):
        pass

    def stat(self, path):
        if path == "__injected.mpy":
            return tuple(0 for _ in range(10))
        else:
            raise OSError(-2)  # ENOENT

    def open(self, path, mode):
        return File()


def mount():
    uos.mount(FS(), "/__remote")
    uos.chdir("/__remote")


def test(r):
    global result
    for _ in r:
        usys.modules.clear()
        module = __import__("__injected")
    result = module.result


###########################################################################
# Benchmark interface

bm_params = {
    (100, 10): (50,),
    (1000, 10): (500,),
    (5000, 10): (5000,),
}


def bm_setup(params):
    (nloop,) = params
    mount()
    return lambda: test(range(nloop)), lambda: (nloop, result)
