# tests for things that only Python 3.6 supports

# underscores in numeric literals
print(100_000)
print(0b1010_0101)
print(0xff_ff)

# underscore supported by int constructor
print(int('1_2_3'))
print(int('0o1_2_3', 8))
