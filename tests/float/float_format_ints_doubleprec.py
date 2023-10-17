# Test formatting of very large ints.
# Relies on double-precision floats.

import array
import sys

# Challenging way to express 1e200 and 1e100.
print("{:.12e}".format(float("9" * 400 + "e-200")))
print("{:.12e}".format(float("9" * 400 + "e-300")))

# These correspond to the binary representation of 1e200 in float64s:
v1 = 0x54B249AD2594C37D  # 1e100
v2 = 0x6974E718D7D7625A  # 1e200
print("{:.12e}".format(array.array("d", v1.to_bytes(8, sys.byteorder))[0]))
print("{:.12e}".format(array.array("d", v2.to_bytes(8, sys.byteorder))[0]))

for i in range(300):
    print(float("1e" + str(i)))
