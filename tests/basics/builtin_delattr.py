# test builtin delattr
try:
    delattr
except:
    import sys
    print("SKIP")
    sys.exit()

class A: pass
a = A()
a.x = 1
print(a.x)

delattr(a, 'x')

try:
    a.x
except AttributeError:
    print('AttributeError')

try:
    delattr(a, 'x')
except AttributeError:
    print('AttributeError')
