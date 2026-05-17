# Test calling non-special method inherited from native type

class mylist(list):
    pass

l = mylist([1, 2, 3])
print(l)
print([e for e in l])


class mylist2(list):

    def __iter__(self):
        return iter([10, 20, 30])

l = mylist2([1, 2, 3])
print(l)
print([e for e in l])
