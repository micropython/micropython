# check cases converting float to int, requiring double precision float

# This case occurs with time.time() values
print(int(1418774543.))
print("%d" % 1418774543.)

testpass = True
for i in range(0,1024):
    bitcnt = len(bin(int(2.**i))) - 3;
    if i != bitcnt:
        print('fail: 2**%u was %u bits long' % (i, bitcnt));
        testpass = False
print("power of  2 test: %s" % (testpass and 'passed' or 'failed'))

testpass = True
for i in range(0,23):
    digcnt = len(str(int(10.**i))) - 1;
    if i != digcnt:
        print('fail: 10**%u was %u digits long' % (i, digcnt));
        testpass = False
print("power of 10 test: %s" % (testpass and 'passed' or 'failed'))
