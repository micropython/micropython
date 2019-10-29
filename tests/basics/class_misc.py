# converting user instance to buffer
class C:
    pass

c = C()
try:
    d = bytes(c)
except TypeError:
    print('TypeError')
