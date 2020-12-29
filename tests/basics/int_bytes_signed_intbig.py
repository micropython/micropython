print((2**64).to_bytes(9, "little",signed=False))

#signed shall be False by default
print((2**64).to_bytes(9, "little"))

# signed=True is not supported yet
try:
    (2**64).to_bytes(9, "little",signed=True)
except NotImplementedError:
    print("NotImplementedError")

try:
    int.from_bytes(b'\xee\xee\xee\xee\xee\xee\xee\xee\xee', "little",signed=True)
except NotImplementedError:
    print("NotImplementedError")

# negative number should raise an error if signed=False
try:
    (- (2**64)).to_bytes(9, "little",signed=False)
except OverflowError:
    print("OverflowError")
