# check cases converting float to int, relying only on single precision float

print(int(14187745.))
print(int(2.**100))
print("%d" % 14187745.)
print("%d" % 2.**100)

testpass = True
for i in range(0,128):
    bitcnt = len(bin(int(2.**i))) - 3;
    if i != bitcnt:
        print('fail: 2**%u was %u bits long' % (i, bitcnt));
        testpass = False
print("power of  2 test: %s" % (testpass and 'passed' or 'failed'))

# TODO why does 10**12 fail this test for single precision float?
testpass = True
for i in range(0,12):
    digcnt = len(str(int(10.**i))) - 1;
    if i != digcnt:
        print('fail: 10**%u was %u digits long' % (i, digcnt));
        testpass = False
print("power of 10 test: %s" % (testpass and 'passed' or 'failed'))

# test inf conversion
try:
    int(float('inf'))
except OverflowError:
    print("OverflowError")

# test nan conversion
try:
    int(float('nan'))
except ValueError:
    print("ValueError")

# test numbers < 1 (this used to fail; see issue #1044)
import struct
struct.pack('I', int(1/2))
