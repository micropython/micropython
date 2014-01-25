# create a class that has a __getitem__ method
class A:
    def __getitem__(self, index):
        print('getitem', index)
        if index > 10:
            raise StopIteration

# test __getitem__
A()[0]
A()[1]

# iterate using a for loop
for i in A():
    pass

# iterate manually
it = iter(A())
try:
    while True:
        next(it)
except StopIteration:
    pass
