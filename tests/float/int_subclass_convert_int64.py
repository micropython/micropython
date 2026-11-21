# test conversion of int subclass instances holding an int64-range
# value to float; unlike int_subclass_convert_intbig.py these values
# fit in 64 bits, so this test also runs with the longlong long-int
# implementation, which has no arbitrary-precision ints


class MyInt(int):
    pass


# use variables so the values are heap int objects at runtime and the
# operations are not constant-folded by mpy-cross; conversion must
# give the same result as for a plain int
pos = 2**40
neg = -(2**40)
print(float(MyInt(pos)) == float(pos))
print(float(MyInt(neg)) == float(neg))

pos = 1234567890123
neg = -1234567890123
print(float(MyInt(pos)) == float(pos))
print(float(MyInt(neg)) == float(neg))

# arithmetic with a float operand
big = 2**40
print(1.5 + MyInt(big) == 1.5 + big)
print(1.5 + MyInt(-big) == 1.5 - big)
