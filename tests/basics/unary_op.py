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
class C(list): pass
print(not C())
