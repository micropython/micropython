# anything above 0xa0 is printed as Unicode by CPython
# the abobe is CPython implementation detail, stick to ASCII
for c in range(0x80):
    print("0x%02x: %s" % (c, repr(chr(c))))
