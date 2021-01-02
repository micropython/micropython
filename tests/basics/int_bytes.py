print((10).to_bytes(1, "little"))
print((111111).to_bytes(4, "little"))
print((100).to_bytes(10, "little"))
print(int.from_bytes(b"\x00\x01\0\0\0\0\0\0", "little"))
print(int.from_bytes(b"\x01\0\0\0\0\0\0\0", "little"))
print(int.from_bytes(b"\x00\x01\0\0\0\0\0\0", "little"))

# check that extra zero bytes don't change the internal int value
print(int.from_bytes(bytes(20), "little") == 0)
print(int.from_bytes(b"\x01" + bytes(20), "little") == 1)

# big-endian conversion
print((10).to_bytes(1, "big"))
print((100).to_bytes(10, "big"))
print(int.from_bytes(b"\0\0\0\0\0\0\0\0\0\x01", "big"))
print(int.from_bytes(b"\x01\0", "big"))

for byteorder in ["little", "big"]:
    for i in [1,2,3,4,5,6,7,8]:
        print((0xff).to_bytes(i, byteorder))
        if i>1:
            print((0xffff).to_bytes(i, byteorder))

    # test max unsigned value
    for i in [1,2,4,8]:
        v = 2**(i*8) - 1
        vbytes = v.to_bytes(i, byteorder)
        print(vbytes)
        print(int.from_bytes(vbytes, byteorder))

    # negative number of bytes should raise an error
    try:
        (1).to_bytes(-1, byteorder)
        print('fail')
    except ValueError:
        print("ValueError")

    # overflow condition
    for i in [1,2,4,8]:
        v = 2**(i*8)
        try:
            vbytes = v.to_bytes(i, byteorder)
            print('fail')
        except OverflowError:
            print('OverflowError')
