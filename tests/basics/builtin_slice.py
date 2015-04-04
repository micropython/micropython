# test builtin slice

# print slice
class A:
    def __getitem__(self, idx):
        print(idx)
A()[1:2:3]
