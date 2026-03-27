# test builtin pow() with integral values
# 2 arg version

print(pow(0, 1))
print(pow(1, 0))
print(pow(-2, 3))
print(pow(3, 8))

# 0**0 is 1 in Python. Literal 0 uses the small-int path; array('q') yields
# multi-precision zero (mp_obj_new_int_from_ll) so ** hits mpz_pow_inpl.
import array

_z = array.array("q", [0])[0]
print(0**0)
print(_z**0)
print(pow(_z, 0))
