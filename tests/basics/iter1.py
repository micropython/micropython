# test user defined iterators

# this class is not iterable
class NotIterable:
    pass
try:
    for i in NotIterable():
        pass
except TypeError:
    print('TypeError')

class MyStopIteration(StopIteration):
    pass

class myiter:
    def __init__(self, i):
        self.i = i

    def __iter__(self):
        return self

    def __next__(self):
        if self.i <= 0:
            # stop in the usual way
            raise StopIteration
        elif self.i == 10:
            # stop with an argument
            raise StopIteration(42)
        elif self.i == 20:
            # raise a non-stop exception
            raise TypeError
        elif self.i == 30:
            # raise a user-defined stop iteration
            print('raising MyStopIteration')
            raise MyStopIteration
        else:
            # return the next value
            self.i -= 1
            return self.i

for i in myiter(5):
    print(i)

for i in myiter(12):
    print(i)

try:
    for i in myiter(22):
        print(i)
except TypeError:
    print('raised TypeError')

try:
    for i in myiter(5):
        print(i)
        raise StopIteration
except StopIteration:
    print('raised StopIteration')

for i in myiter(32):
    print(i)
