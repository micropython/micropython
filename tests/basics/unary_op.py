x = 1
print(+x)
print(-x)
print(~x)

print(not None)
print(not False)
print(not True)
print(not 0)
print(not 1)
print(not -1)
print(not ())
print(not (1,))
print(not [])
print(not [1,])
print(not {})
print(not {1:1})

# check user instance
class A: pass
print(not A())

# check user instances derived from builtins
class B(int): pass
print(not B())
print(True if B() else False)
class C(list): pass
print(not C())
print(True if C() else False)
# type doesn't define unary_op
class D(type): pass
d = D("foo", (), {})
print(not d)
print(True if d else False)
