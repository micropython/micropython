# test ustruct with a count specified before the type

try:
    import ustruct as struct
except:
    try:
        import struct
    except ImportError:
        print("SKIP")
        raise SystemExit

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

# check that we get an error if the buffer is too small
try:
    struct.unpack('2H', b'\x00\x00')
except:
    print('Exception')
try:
    struct.pack_into('2I', bytearray(4), 0, 0)
except:
    print('Exception')

# check that unknown types raise an exception
try:
    struct.unpack('z', b'1')
except:
    print('Exception')

try:
    struct.pack('z', (b'1',))
except:
    print('Exception')

try:
    struct.calcsize('0z')
except:
    print('Exception')

# check that a count without a type specifier raises an exception

try:
    struct.calcsize('1')
except:
    print('Exception')

try:
    struct.pack('1')
except:
    print('Exception')

try:
    struct.pack_into('1', bytearray(4), 0, 'xx')
except:
    print('Exception')

try:
    struct.unpack('1', 'xx')
except:
    print('Exception')

try:
    struct.unpack_from('1', 'xx')
except:
    print('Exception')
