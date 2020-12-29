print((255).to_bytes(1, "little",signed=False))

#signed shall be False by default
print((255).to_bytes(1, "little"))

# signed=True is not supported yet
try:
    (1).to_bytes(1, "little",signed=True)
except NotImplementedError:
    print("NotImplementedError")

try:
    int.from_bytes(b'\xff', "little",signed=True)
except NotImplementedError:
    print("NotImplementedError")

# negative number should raise an error if signed=False
try:
    (-1).to_bytes(1, "little",signed=False)
except OverflowError:
    print("OverflowError")
