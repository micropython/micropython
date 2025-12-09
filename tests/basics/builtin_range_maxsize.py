try:
    from sys import maxsize
except ImportError:
    print("SKIP")
    raise SystemExit

# Test the range builtin at extreme values. (https://github.com/micropython/micropython/issues/17684)
#
# This is written using asserts instead of prints because the value of `maxsize` differs.
#
# Numbers & counts right up against the max of mp_int_t also cause overflows, such as
# objrange.c:115:14: runtime error: signed integer overflow: 9223372036854775807 + 1 cannot be represented in type 'long int'
# so just avoid them for the purposes of this test.

r = range(-maxsize + 1, -1)
assert r.start == -maxsize + 1
assert r.stop == -1
assert r[0] == -maxsize + 1
assert r[1] == -maxsize + 2
assert r[-1] == -2
assert r[-2] == -3

ir = iter(r)
assert next(ir) == -maxsize + 1
assert next(ir) == -maxsize + 2

r = range(0, maxsize - 1)
assert len(r) == maxsize - 1
assert r.stop == maxsize - 1

r = range(maxsize, 0, -1)
assert len(r) == maxsize
assert r.start == maxsize
assert r[0] == maxsize
assert r[1] == maxsize - 1
ir = iter(r)
assert next(ir) == maxsize
assert next(ir) == maxsize - 1
