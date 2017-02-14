class Descriptor:
    def __get__(self, obj, cls):
        print('get')
        print(type(obj) is Main)
        print(cls is Main)
        return 'result'

    def __set__(self, obj, val):
        print('set')
        print(type(obj) is Main)
        print(val)

    def __delete__(self, obj):
        print('delete')
        print(type(obj) is Main)

class Main:
    Forward = Descriptor()

m = Main()
try:
    m.__class__
except AttributeError:
    import sys
    print("SKIP")
    sys.exit()

r = m.Forward
if 'Descriptor' in repr(r.__class__):
    print('SKIP')
else:
    print(r)
    m.Forward = 'a'
    del m.Forward

