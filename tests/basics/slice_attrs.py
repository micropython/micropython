# test builtin slice attributes access

# print slice attributes
class A:
    def __getitem__(self, idx):
        print(idx.start, idx.stop, idx.step)
A()[1:2:3]
