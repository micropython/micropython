# test builtin slice attributes access

# print slice attributes
class A:
    def __getitem__(self, idx):
        print(idx.start, idx.stop, idx.step)

try:
    t = A()[1:2]
except:
    import sys
    print("SKIP")
    sys.exit()


A()[1:2:3]
