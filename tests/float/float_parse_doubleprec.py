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
print(float('.' + '0' * 400 + '9e100'))
print(float('.' + '0' * 400 + '9e200'))
print(float('.' + '0' * 400 + '9e400'))
