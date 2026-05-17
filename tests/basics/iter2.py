# user defined iterator used in something other than a for loop

class MyStopIteration(StopIteration):
    pass

class myiter:
    def __init__(self, i):
        self.i = i

    def __iter__(self):
        return self

    def __next__(self):
        if self.i == 0:
            raise StopIteration
        elif self.i == 1:
            raise StopIteration(1)
        elif self.i == 2:
            raise MyStopIteration

print(list(myiter(0)))
print(list(myiter(1)))
print(list(myiter(2)))
