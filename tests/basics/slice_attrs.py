# test builtin slice attributes access

# print slice attributes
class A:
    def __getitem__(self, idx):
        print(idx.start, idx.stop, idx.step)

# The skip test method for qemu-arm/test-main.c requires we test
# for the feature being testable and bail thusly. It does not use
# the tests/feature_check mechanism used by tests/run-tests
try:
    t = a()[1:2]
except:
    import sys
    print("SKIP")
    sys.exit()


A()[1:2:3]
