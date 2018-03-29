# test parsing of floats, requiring double-precision

# very large integer part with a very negative exponent should cancel out
print(float('9' * 400 + 'e-100'))
print(float('9' * 400 + 'e-200'))
print(float('9' * 400 + 'e-400'))

# many fractional digits
print(float('.' + '9' * 400))
print(float('.' + '9' * 400 + 'e100'))
print(float('.' + '9' * 400 + 'e-100'))

# tiny fraction with large exponent
print('%.14e' % float('.' + '0' * 400 + '9e100'))
print('%.14e' % float('.' + '0' * 400 + '9e200'))
print('%.14e' % float('.' + '0' * 400 + '9e400'))

# ensure that accuracy is retained when value is close to a subnormal
print(float('1.00000000000000000000e-307'))
print(float('10.0000000000000000000e-308'))
print(float('100.000000000000000000e-309'))
