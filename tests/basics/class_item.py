# test class with __getitem__, __setitem__, __delitem__ methods

class C:
    def __getitem__(self, item):
        print('get', item)
        return 'item'

    def __setitem__(self, item, value):
        print('set', item, value)

    def __delitem__(self, item):
        print('del', item)

c = C()
print(c[1])
c[1] = 2
del c[3]
