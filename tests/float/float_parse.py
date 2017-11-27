# test parsing of floats

inf = float('inf')

# it shouldn't matter where the decimal point is if the exponent balances the value
print(float('1234') - float('0.1234e4'))
print(float('1.015625') - float('1015625e-6'))

# very large integer part with a very negative exponent should cancel out
print(float('9' * 60 + 'e-60'))
print(float('9' * 60 + 'e-40'))
print(float('9' * 60 + 'e-20') == float('1e40'))

# many fractional digits
print(float('.' + '9' * 70))
print(float('.' + '9' * 70 + 'e20'))
print(float('.' + '9' * 70 + 'e-50') == float('1e-50'))

# tiny fraction with large exponent
print(float('.' + '0' * 60 + '1e10') == float('1e-51'))
print(float('.' + '0' * 60 + '9e25'))
print(float('.' + '0' * 60 + '9e40'))
