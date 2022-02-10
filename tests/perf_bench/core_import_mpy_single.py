# Test performance of importing an .mpy file just once.
# The first import of a module will intern strings that don't already exist, and
# this test should be representative of what happens in a real application.

import uio, uos

if not (hasattr(uio, "IOBase") and hasattr(uos, "mount")):
    print("SKIP")
    raise SystemExit

# This is the test.py file that is compiled to test.mpy below.
# Many known and unknown names/strings are included to test the linking process.
"""
class A0:
    def a0(self): pass
    def a1(self): pass
    def a2(self): pass
    def a3(self): pass
class A1:
    def a0(self): pass
    def a1(self): pass
    def a2(self): pass
    def a3(self): pass
def f0():
    __call__, __class__, __delitem__, __enter__, __exit__, __getattr__, __getitem__,
    __hash__, __init__, __int__, __iter__, __len__, __main__, __module__, __name__,
    __new__, __next__, __qualname__, __repr__, __setitem__, __str__,
    ArithmeticError, AssertionError, AttributeError, BaseException, EOFError, Ellipsis,
    Exception, GeneratorExit, ImportError, IndentationError, IndexError, KeyError,
    KeyboardInterrupt, LookupError, MemoryError, NameError, NoneType,
    NotImplementedError, OSError, OverflowError, RuntimeError, StopIteration,
    SyntaxError, SystemExit, TypeError, ValueError, ZeroDivisionError,
    abs, all, any, append, args, bool, builtins, bytearray, bytecode, bytes, callable,
    chr, classmethod, clear, close, const, copy, count, dict, dir, divmod, end,
    endswith, eval, exec, extend, find, format, from_bytes, get, getattr, globals,
    hasattr, hash, id, index, insert, int, isalpha, isdigit, isinstance, islower,
    isspace, issubclass, isupper, items, iter, join, key, keys, len, list, little,
    locals, lower, lstrip, main, map, micropython, next, object, open, ord, pop,
    popitem, pow, print, range, read, readinto, readline, remove, replace, repr,
    reverse, rfind, rindex, round, rsplit, rstrip, self, send, sep, set, setattr,
    setdefault, sort, sorted, split, start, startswith, staticmethod, step, stop, str,
    strip, sum, super, throw, to_bytes, tuple, type, update, upper, value, values,
    write, zip,
    name0, name1, name2, name3, name4, name5, name6, name7, name8, name9,
    quite_a_long_name0, quite_a_long_name1, quite_a_long_name2, quite_a_long_name3,
    quite_a_long_name4, quite_a_long_name5, quite_a_long_name6, quite_a_long_name7,
    quite_a_long_name8, quite_a_long_name9, quite_a_long_name10, quite_a_long_name11,
def f1():
    x = "this will be a string object 0"
    x = "this will be a string object 1"
    x = "this will be a string object 2"
    x = "this will be a string object 3"
    x = "this will be a string object 4"
    x = "this will be a string object 5"
    x = "this will be a string object 6"
    x = "this will be a string object 7"
    x = "this will be a string object 8"
    x = "this will be a string object 9"
    x = b"this will be a bytes object 0"
    x = b"this will be a bytes object 1"
    x = b"this will be a bytes object 2"
    x = b"this will be a bytes object 3"
    x = b"this will be a bytes object 4"
    x = b"this will be a bytes object 5"
    x = b"this will be a bytes object 6"
    x = b"this will be a bytes object 7"
    x = b"this will be a bytes object 8"
    x = b"this will be a bytes object 9"
    x = ("const tuple 0", None, False, True, 1, 2, 3)
    x = ("const tuple 1", None, False, True, 1, 2, 3)
    x = ("const tuple 2", None, False, True, 1, 2, 3)
    x = ("const tuple 3", None, False, True, 1, 2, 3)
    x = ("const tuple 4", None, False, True, 1, 2, 3)
    x = ("const tuple 5", None, False, True, 1, 2, 3)
    x = ("const tuple 6", None, False, True, 1, 2, 3)
    x = ("const tuple 7", None, False, True, 1, 2, 3)
    x = ("const tuple 8", None, False, True, 1, 2, 3)
    x = ("const tuple 9", None, False, True, 1, 2, 3)
result = 123
"""
file_data = b'M\x05\x02\x1f \x81\\\x10\x1a\x00\x07\x0etest.pyk@k@\x85\x18\x85\x1f\x00T2\x00\x10\x04A04\x02\x16\x01T2\x01\x10\x04A14\x02\x16\x012\x02\x16\x04f02\x03\x16\x04f1"\x80{\x16\x0cresultQc\x00\x04\x814\x00\x12\t\x0b,%%%\x00\x11\x00\x17\x16\x00\x16\x10\x03\x16\x00\x1a2\x00\x16\x04a02\x01\x16\x04a12\x02\x16\x04a22\x03\x16\x04a3Qc\x00\x04(\t\x0c\x07\x0b \x00Qc\x00\x00\x00\x89(\t\x0c\t\x03@\x00Qc\x00\x00\x00\x89(\t\x0c\t\x03`\x00Qc\x00\x00\x00\x89,\t\x0e\t\x03` \x00Qc\x00\x00\x00\x89\x818\x00\x14\x13\x03l`%%%\x00\x11\x00\x17\x16\x00\x16\x10\x03\x16\x00\x1a2\x00\x16\x0b2\x01\x16\x0b2\x02\x16\x0b2\x03\x16\x0bQc\x00\x04,\t\x0e\x07\x0b``\x00Qc\x00\x00\x00\x89,\t\x0e\t\x03\x80\x07\x00Qc\x00\x00\x00\x89,\t\x0e\t\x03\x80\x08\x00Qc\x00\x00\x00\x89,\t\x0e\t\x03\x80\t\x00Qc\x00\x00\x00\x89\x94\x1cPP\x13\x03\x80\x0b8;555222\x1f%\x1f%\x1f"\x1f"\x1f%\x1f%\x1f"\x1f%\x1f"\x1f%)\x1f"//\x00\x12\x00\t\x12\x00\n\x12\x00\x0b\x12\x00\x0c\x12\x00\r\x12\x00\x0e\x12\x00\x0f*\x07Y\x12\x00\x10\x12\x00\x11\x12\x00\x12\x12\x00\x13\x12\x00\x14\x12\x00\x15\x12\x00\x16\x12\x00\x17*\x08Y\x12\x00\x18\x12\x00\x19\x12\x00\x1a\x12\x00\x1b\x12\x00\x1c\x12\x00\x1d*\x06Y\x12\x00\x1e\x12\x00\x1f\x12\x00 \x12\x00!\x12\x00"\x12\x00#*\x06Y\x12\x00$\x12\x00%\x12\x00&\x12\x00\'\x12\x00(\x12\x00)*\x06Y\x12\x00*\x12\x00+\x12\x00,\x12\x00-\x12\x00.*\x05Y\x12\x00/\x12\x000\x12\x001\x12\x002\x12\x003*\x05Y\x12\x004\x12\x005\x12\x006\x12\x007\x12\x008*\x05Y\x12\x009\x12\x00:\x12\x00;\x12\x00<\x12\x00=\x12\x00>\x12\x00?\x12\x00@\x12\x00A\x12\x00B\x12\x00C*\x0bY\x12\x00D\x12\x00E\x12\x00F\x12\x00G\x12\x00H\x12\x00I\x12\x00J\x12\x00K\x12\x00L\x12\x00M\x12\x00N*\x0bY\x12\x00O\x12\x00P\x12\x00Q\x12\x00R\x12\x00S\x12\x00T\x12\x00U\x12\x00V\x12\x00W\x12\x00X*\nY\x12\x00Y\x12\x00Z\x12\x00[\x12\x00\\\x12\x00]\x12\x00^\x12\x00_\x12\x00`\x12\x00a\x12\x00b*\nY\x12\x00c\x12\x00d\x12\x00e\x12\x00f\x12\x00g\x12\x00h\x12\x00i\x12\x00j\x12\x00k\x12\x00l\x12\x00m*\x0bY\x12\x00n\x12\x00o\x12\x00p\x12\x00q\x12\x00r\x12\x00s\x12\x00t\x12\x00u\x12\x00v\x12\x00w\x12\x00x*\x0bY\x12\x00y\x12\x00z\x12\x00{\x12\x00|\x12\x00}\x12\x00~\x12\x00\x7f\x12\x00\x80\x12\x00\x81\x12\x00\x82*\nY\x12\x00\x83\x12\x00\x84\x12\x00\x85\x12\x00\x86\x12\x00\x87\x12\x00\x88\x12\x00\x89\x12\x00\x8a\x12\x00\x8b\x12\x00\x8c\x12\x00\x8d*\x0bY\x12\x00\x8e\x12\x00\x8f\x12\x00\x90\x12\x00\x91\x12\x00\x92\x12\x00\x93\x12\x00\x94\x12\x00\x95\x12\x00\x96\x12\x00\x97*\nY\x12\x00\x98\x12\x00\x99\x12\x00\x9a\x12\x00\x9b\x12\x00\x9c\x12\x00\x9d\x12\x00\x9e\x12\x00\x9f\x12\x00\xa0\x12\x00\xa2\x12\x00\xa3*\x0bY\x12\x00\xa4\x12\x00\xa5*\x02Y\x12\nname0\x12\nname1\x12\nname2\x12\nname3\x12\nname4\x12\nname5\x12\nname6\x12\nname7\x12\nname8\x12\nname9*\nY\x12$quite_a_long_name0\x12$quite_a_long_name1\x12$quite_a_long_name2\x12$quite_a_long_name3*\x04Y\x12$quite_a_long_name4\x12$quite_a_long_name5\x12$quite_a_long_name6\x12$quite_a_long_name7*\x04Y\x12$quite_a_long_name8\x12$quite_a_long_name9\x12&quite_a_long_name10\x12&quite_a_long_name11*\x04YQc\x00\x00\x86H8H?/\x80#####################+++++++++\x00#\x00\xc0#\x01\xc0#\x02\xc0#\x03\xc0#\x04\xc0#\x05\xc0#\x06\xc0#\x07\xc0#\x08\xc0#\t\xc0#\n\xc0#\x0b\xc0#\x0c\xc0#\r\xc0#\x0e\xc0#\x0f\xc0#\x10\xc0#\x11\xc0#\x12\xc0#\x13\xc0#\x14QPR\x81\x82\x83*\x07\xc0#\x15QPR\x81\x82\x83*\x07\xc0#\x16QPR\x81\x82\x83*\x07\xc0#\x17QPR\x81\x82\x83*\x07\xc0#\x18QPR\x81\x82\x83*\x07\xc0#\x19QPR\x81\x82\x83*\x07\xc0#\x1aQPR\x81\x82\x83*\x07\xc0#\x1bQPR\x81\x82\x83*\x07\xc0#\x1cQPR\x81\x82\x83*\x07\xc0#\x1dQPR\x81\x82\x83*\x07\xc0Qc\x1e\x00s\x1ethis will be a string object 0s\x1ethis will be a string object 1s\x1ethis will be a string object 2s\x1ethis will be a string object 3s\x1ethis will be a string object 4s\x1ethis will be a string object 5s\x1ethis will be a string object 6s\x1ethis will be a string object 7s\x1ethis will be a string object 8s\x1ethis will be a string object 9b\x1dthis will be a bytes object 0b\x1dthis will be a bytes object 1b\x1dthis will be a bytes object 2b\x1dthis will be a bytes object 3b\x1dthis will be a bytes object 4b\x1dthis will be a bytes object 5b\x1dthis will be a bytes object 6b\x1dthis will be a bytes object 7b\x1dthis will be a bytes object 8b\x1dthis will be a bytes object 9s\rconst tuple 0s\rconst tuple 1s\rconst tuple 2s\rconst tuple 3s\rconst tuple 4s\rconst tuple 5s\rconst tuple 6s\rconst tuple 7s\rconst tuple 8s\rconst tuple 9'


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


def test():
    global result
    module = __import__("__injected")
    result = module.result


###########################################################################
# Benchmark interface

bm_params = {
    (1, 1): (),
}


def bm_setup(params):
    mount()
    return lambda: test(), lambda: (1, result)
