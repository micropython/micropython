# Tests various constants of the math module.

import sys

try:
    from array import array
    from math import e, pi
except ImportError:
    print("SKIP")
    raise SystemExit

# Hexadecimal representations of e and pi constants.
e_truth_single = 0x402DF854
pi_truth_single = 0x40490FDB
e_truth_double = 0x4005BF0A8B145769
pi_truth_double = 0x400921FB54442D18

# Detect the floating-point precision of the system, to determine the exact values of
# the constants (parsing the float from a decimal string can lead to inaccuracies).
if float("1e300") == float("inf"):
    # Single precision floats.
    e_truth = array("f", e_truth_single.to_bytes(4, sys.byteorder))[0]
    pi_truth = array("f", pi_truth_single.to_bytes(4, sys.byteorder))[0]
else:
    # Double precision floats.
    e_truth = array("d", e_truth_double.to_bytes(8, sys.byteorder))[0]
    pi_truth = array("d", pi_truth_double.to_bytes(8, sys.byteorder))[0]

print("e:", e == e_truth or (e, e_truth, e - e_truth))
print("pi:", pi == pi_truth or (pi, pi_truth, pi - pi_truth))
