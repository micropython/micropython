# converting user instance to buffer
class C:
    pass

c = C()
try:
    d = bytearray(c)
except TypeError:
    print('TypeError')
