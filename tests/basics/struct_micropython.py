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
import uctypes

class A():
    pass

# pack and unpack objects
o = A()
s = struct.pack("<O", o)
o2 = struct.unpack("<O", s)
print(o is o2[0])

 # get_val for pointer to string
o = uctypes.addressof('abc')
s = struct.pack("<S", o)
o2 = struct.unpack("<S", s)
print('abc' == o2[0])
