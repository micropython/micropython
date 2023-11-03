# Inspired by urlparse.py from CPython 3.3 stdlib
# There was a bug in MicroPython that under some conditions class stored
# in instance attribute later was returned "bound" as if it was a method,
# which caused class constructor to receive extra argument.
try:
    from collections import namedtuple
except ImportError:
    print("SKIP")
    raise SystemExit

_DefragResultBase = namedtuple('DefragResult', [ 'foo', 'bar' ])

class _ResultMixinStr(object):
    def encode(self):
        return self._encoded_counterpart(*(x.encode() for x in self))

class _ResultMixinBytes(object):
    def decode(self):
        return self._decoded_counterpart(*(x.decode() for x in self))

class DefragResult(_DefragResultBase, _ResultMixinStr):
    pass

class DefragResultBytes(_DefragResultBase, _ResultMixinBytes):
    pass


DefragResult._encoded_counterpart = DefragResultBytes
DefragResultBytes._decoded_counterpart = DefragResult

# Due to differences in type and native subclass printing,
# the best thing we can do here is to just test that no exceptions
# happen

#print(DefragResult, DefragResult._encoded_counterpart)
#print(DefragResultBytes, DefragResultBytes._decoded_counterpart)

o1 = DefragResult("a", "b")
#print(o1, type(o1))
o2 = DefragResultBytes("a", "b")
#print(o2, type(o2))

#print(o1._encoded_counterpart)
_o1 = o1.encode()
print(_o1[0], _o1[1])
#print(_o1, type(_o1))

print("All's ok")
