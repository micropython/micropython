# test float formatting

# general rounding
for val in (116, 1111, 1234, 5010, 11111):
    print("%.0f" % val)
    print("%.1f" % val)
    print("%.3f" % val)

# make sure rounding is done at the correct precision
for prec in range(8):
    print(("%%.%df" % prec) % 6e-5)

# check certain cases that had a digit value of 10 render as a ":" character
print("%.2e" % float("9" * 51 + "e-39"))
print("%.2e" % float("9" * 40 + "e-21"))

# check a case that would render negative digit values, eg ")" characters
# the string is converted back to a float to check for no illegal characters
float("%.23e" % 1e-80)

# Check a problem with malformed "e" format numbers on the edge of 1.0e-X.
for r in range(38):
    s = "%.12e" % float("1e-" + str(r))
    # It may format as 1e-r, or 9.999...e-(r+1), both are OK.
    # But formatting as 0.999...e-r is NOT ok.
    if s[0] == "0":
        print("FAIL:", s)
