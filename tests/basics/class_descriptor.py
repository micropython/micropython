class Descriptor:
    def __get__(self, obj, cls):
        print(type(obj) is Main)
        print(cls is Main)
        return 'result'

    def __set__(self, obj, val):
        print(type(obj) is Main)
        print(val)

class Main:
    Forward = Descriptor()

m = Main()
r = m.Forward
if 'Descriptor' in repr(r.__class__):
    print('SKIP')
else:
    print(r)
    m.Forward = 'a'

