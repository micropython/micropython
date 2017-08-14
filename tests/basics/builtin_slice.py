# test builtin slice

# print slice
class A:
    def __getitem__(self, idx):
        print(idx)
        return idx
s = A()[1:2:3]

# check type
print(type(s) is slice)
