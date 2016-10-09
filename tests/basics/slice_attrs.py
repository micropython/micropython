# test builtin slice attributes access

# get a slice
class A:
    def __getitem__(self, idx):
        return idx

try:
    s = A()[1:2]
    print(s.start, s.stop, s.step)
except:
    import sys
    print("SKIP")
    sys.exit()


s = A()[1:2:3]
print(s.start, s.stop, s.step)
s = A()[1:5]
print(s.indices(9))
print(s.indices(4))
s = A()[-3:-1]
print(s.indices(9))
