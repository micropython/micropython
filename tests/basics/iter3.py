# user defined iterator used in something other than a for loop

class MyStopIteration(StopIteration):
    pass

class mygettable():
    def __init__(self, iterable):
        self.values = list(iterable)

    def __len__(self):
        return len(self.values)

    def __getitem__(self, index):
        return self.values[index]

    def __setitem__(self, index, value):
        self.values[index] = value

print(list(mygettable(range(3))))
