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
file_data = b'M\x05\x02\x1f \x81\x0c\x10\x12\x00\x07\x0etest.py\x8b\x07e`\x00T2\x00\x10\x02A4\x02\x16\x012\x01\x16\x02f"\x80{\x16\x0cresultQc\x00\x02\x81\x1c\x00\x10\x05\x07,EE\x00\x11\x00\x17\x16\x00\x16\x10\x03\x16\x00\x1a2\x00\x16\x00\x112\x01\x16\x00\xa42\x02\x16\x00}Qc\x00\x03<\x1a\x0c\x00\x11\x03@\x00\xb1\xb0\x18\x06argQc\x00\x00\x00\x89\x01,\t\x0e\x00\xa4\x03` \x00Qc\x00\x00\x00\x89,\t\x0e\x00}\x01``\x00Qc\x00\x00\x00\x89\x81l8\x16\t\x03\x80\x08/,##\x00\x12\x00{\x12\x00\x97\x12\x00B\x12\x00K*\x04Y\x12\x00$\x12\x007\x12\x006*\x03Y#\x00\xc0#\x01\xc0#\x02QPR\x81\x82\x83*\x07\xc0Qc\x03\x00s\x1cthis will be a string objectb\x1bthis will be a bytes objects\x0bconst tuple'


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
