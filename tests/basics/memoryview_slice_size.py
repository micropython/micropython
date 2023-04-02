# test memoryview slicing beyond the limit of what memoryview can internally index

try:
    from sys import maxsize
    from uctypes import bytearray_at

    memoryview
except:
    print("SKIP")
    raise SystemExit

if maxsize <= 0xFFFF_FFFF:
    slice_max = 0xFF_FFFF
else:
    slice_max = 0xFF_FFFF_FFFF_FFFF

buf = bytearray_at(0, slice_max + 2)
mv = memoryview(buf)

# this should work
print(mv[slice_max : slice_max + 1])

# this should overflow the internal index for memoryview slicing
try:
    print(mv[slice_max + 1 : slice_max + 2])
except OverflowError:
    print("OverflowError")
