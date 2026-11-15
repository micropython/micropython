# test builtin delattr
try:
    delattr
except:
    print("SKIP")
    raise SystemExit

class A: pass
class S(str): pass
a = A()
a.x = 1
print(a.x)

delattr(a, S('x'))

try:
    a.x
except AttributeError:
    print('AttributeError')

try:
    delattr(a, 'x')
except AttributeError:
    print('AttributeError')
