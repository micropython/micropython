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
