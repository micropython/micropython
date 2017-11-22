# test MicroPython-specific features of struct

try:
    import ustruct as struct
except:
    try:
        import struct
    except ImportError:
        print("SKIP")
        raise SystemExit

class A():
    pass

# pack and unpack objects
o = A()
s = struct.pack("<O", o)
o2 = struct.unpack("<O", s)
print(o is o2[0])

# pack can accept less arguments than required for the format spec
print(struct.pack('<2I', 1))

# pack and unpack pointer to a string
# This requires uctypes to get the address of the string and instead of
# putting this in a dedicated test that can be skipped we simply pass
# if the import fails.
try:
    import uctypes
    o = uctypes.addressof('abc')
    s = struct.pack("<S", o)
    o2 = struct.unpack("<S", s)
    assert o2[0] == 'abc'
except ImportError:
    pass
