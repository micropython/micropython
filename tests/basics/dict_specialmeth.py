# dict object with special methods

d = {}
d.__setitem__('2', 'two')
print(d.__getitem__('2'))
d.__delitem__('2')
print(d)
