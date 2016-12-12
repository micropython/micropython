import uctypes

# Tests extmod/moductypes.c

desc = {
    "arr": (uctypes.ARRAY | 0, uctypes.UINT8 | 1)
}
S = uctypes.struct(0, desc)
# lines 137, 142, 154
print(S)

desc2 = [(uctypes.ARRAY | 0, uctypes.UINT8 | 1)]
S2 = uctypes.struct(0, desc2)
# line 152
print(S2)

desc3 = ((uctypes.ARRAY | 0, uctypes.UINT8 | 1))
S3 = uctypes.struct(0, desc3)
#lines 144, 149
print(S3)

desc4 = ((uctypes.PTR | 0, uctypes.UINT8 | 1))
S4 = uctypes.struct(0, desc4)
#lines 148
print(S4)
