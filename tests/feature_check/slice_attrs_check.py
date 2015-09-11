class A:
    def __getitem__(self, idx):
        idx.start

try:
    A()[1:2:3]
except AttributeError:
    print('False')
else:
    print('True')
