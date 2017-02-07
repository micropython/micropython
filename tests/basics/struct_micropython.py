# test MicroPython-specific features of struct

try:
    import ustruct as struct
except:
    try:
        import struct
    except ImportError:
        import sys
        print("SKIP")
        sys.exit()

class A():
    pass

# pack and unpack objects
o = A()
s = struct.pack("<O", o)
o2 = struct.unpack("<O", s)
print(o is o2[0])
