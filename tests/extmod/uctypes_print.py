# test printing of uctypes objects

import uctypes

# we use an address of "0" because we just want to print something deterministic
# and don't actually need to set/get any values in the struct

desc = {"arr": (uctypes.ARRAY | 0, uctypes.UINT8 | 1)}
S = uctypes.struct(0, desc)
print(S)

desc2 = [(uctypes.ARRAY | 0, uctypes.UINT8 | 1)]
S2 = uctypes.struct(0, desc2)
print(S2)

desc3 = ((uctypes.ARRAY | 0, uctypes.UINT8 | 1))
S3 = uctypes.struct(0, desc3)
print(S3)

desc4 = ((uctypes.PTR | 0, uctypes.UINT8 | 1))
S4 = uctypes.struct(0, desc4)
print(S4)
