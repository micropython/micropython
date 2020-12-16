# check cases converting float to int, relying only on single precision float

try:
    import ustruct as struct
except:
    import struct

import sys
maxsize_bits = 0
maxsize = sys.maxsize
while maxsize:
    maxsize >>= 1
    maxsize_bits += 1

# work out configuration values
is_64bit = maxsize_bits > 32
# 0 = none, 1 = long long, 2 = mpz
ll_type = None
if is_64bit:
    if maxsize_bits < 63:
        ll_type = 0
else:
    if maxsize_bits < 31:
        ll_type = 0
if ll_type is None:
    one = 1
    if one << 65 < one << 62:
        ll_type = 1
    else:
        ll_type = 2

# basic conversion
print(int(14187744.))
print("%d" % 14187744.)
if ll_type == 2:
    print(int(2.**100))
    print("%d" % 2.**100)

testpass = True
p2_rng = ((30,63,127),(62,63,127))[is_64bit][ll_type]
for i in range(0,p2_rng):
    bitcnt = len(bin(int(2.**i))) - 3;
    if i != bitcnt:
        print('fail: 2.**%u was %u bits long' % (i, bitcnt));
        testpass = False
print("power of  2 test: %s" % (testpass and 'passed' or 'failed'))

# TODO why does 10**12 fail this test for single precision float?
testpass = True
p10_rng = 9
for i in range(0,p10_rng):
    digcnt = len(str(int(10.**i))) - 1;
    if i != digcnt:
        print('fail: 10.**%u was %u digits long' % (i, digcnt));
        testpass = False
print("power of 10 test: %s" % (testpass and 'passed' or 'failed'))

def fp2int_test(num, name, should_fail):
    try:
        x = int(num)
        passed = ~should_fail
    except:
        passed = should_fail
    print('%s: %s' % (name, passed and 'passed' or 'failed'))

if ll_type != 2:
    if ll_type == 0:
        if is_64bit:
            neg_bad_fp = -1.00000005*2.**62.
            pos_bad_fp = 2.**62.
            neg_good_fp = -2.**62.
            pos_good_fp = 0.99999993*2.**62.
        else:
            neg_bad_fp = -1.00000005*2.**30.
            pos_bad_fp = 2.**30.
            neg_good_fp = -2.**30.
            pos_good_fp = 0.9999999499*2.**30.
    else:
        neg_bad_fp = -0.51*2.**64.
        pos_bad_fp = 2.**63.
        neg_good_fp = -2.**63.
        pos_good_fp = 1.9999998*2.**62.

    fp2int_test(neg_bad_fp, 'neg bad', True)
    fp2int_test(pos_bad_fp, 'pos bad', True)
    fp2int_test(neg_good_fp, 'neg good', False)
    fp2int_test(pos_good_fp, 'pos good', False)
else:
    fp2int_test(-1.999999879*2.**126., 'large neg', False)
    fp2int_test(1.999999879*2.**126., 'large pos', False)

fp2int_test(float('inf'), 'inf test', True)
fp2int_test(float('nan'), 'NaN test', True)

# test numbers < 1 (this used to fail; see issue #1044)
fp2int_test(0.0001, 'small num', False)
struct.pack('I', int(1/2))
