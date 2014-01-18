# test class with __getitem__ and __setitem__ methods

class C:
    def __getitem__(self, item):
        print('get', item)
        return 'item'

    def __setitem__(self, item, value):
        print('set', item, value)

c = C()
print(c[1])
c[1] = 2
