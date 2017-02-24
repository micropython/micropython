# test builtin slice

# print slice
class A:
    def __getitem__(self, idx):
        print(idx)
        return idx
s = A()[1:2:3]

# check type
print(type(s) is slice)

s = slice(10)
print(s)

s = slice(0, 4)
print(s)

s = slice(0, 4, 2)
print(s)

s = slice(-1)
print(s)
print(s.indices(10))

s = slice(-5, -1)
print(s)
print(s.indices(10))

s = slice(-100, -2, -1)
print(s)
print(s.indices(10))

s = slice(None, None, -2)
print(s)
print(s.indices(10))

s = slice(-100, -2, 0)
print(s)
try:
    print(s.indices(10))
except Exception as e:
    print(e)
