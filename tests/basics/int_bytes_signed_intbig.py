print((2**64).to_bytes(9, "little",signed=False))

#signed shall be False by default
print((2**64).to_bytes(9, "little"))

# test min/max signed value
for i in [10]:
    v = 2**(i*8 - 1) - 1
    print(v)
    vbytes = v.to_bytes(i, "little")
    print(vbytes)
    print(int.from_bytes(vbytes,"little"))
    v = - (2**(i*8 - 1))
    print(v)
    vbytes = v.to_bytes(i, "little",signed=True)
    print(vbytes)
    print(int.from_bytes(vbytes,"little",signed=True))

# negative number should raise an error if signed=False
try:
    (- (2**64)).to_bytes(9, "little",signed=False)
except OverflowError:
    print("OverflowError")
