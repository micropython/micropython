# test ustruct with a count specified before the type

try:
    import ustruct as struct
except:
    try:
        import struct
    except ImportError:
        import sys
        print("SKIP")
        sys.exit()

print(struct.calcsize('0s'))
print(struct.unpack('0s', b''))
print(struct.pack('0s', b'123'))

print(struct.calcsize('2s'))
print(struct.unpack('2s', b'12'))
print(struct.pack('2s', b'123'))

print(struct.calcsize('2H'))
print(struct.unpack('<2H', b'1234'))
print(struct.pack('<2H', 258, 515))

print(struct.calcsize('0s1s0H2H'))
print(struct.unpack('<0s1s0H2H', b'01234'))
print(struct.pack('<0s1s0H2H', b'abc', b'abc', 258, 515))

# check that zero of an unknown type raises an exception
try:
    struct.calcsize('0z')
except:
    print('Exception')
