# test float formatting

# general rounding
for val in (116, 1111, 1234, 5010, 11111):
    print('%.0f' % val)
    print('%.1f' % val)
    print('%.3f' % val)

# make sure rounding is done at the correct precision
for prec in range(8):
    print(('%%.%df' % prec) % 6e-5)
