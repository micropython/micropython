import bench


def test(num):
    buf = bytearray(16)
    a = b"\x00\x01\x02\x03\x04\x05\x06\x07"
    i = 0
    while i < 20000000:
        # slice the starting index of lvalue and rvalue
        buf[8:] = a[0:]
        i += 1


bench.run(test)
