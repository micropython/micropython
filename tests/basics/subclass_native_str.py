# Test subclassing built-in str

class S(str):
    pass

s = S('hello')
print(s == 'hello')
print('hello' == s)
print(s == 'Hello')
print('Hello' == s)

# str subclass as an attribute name
class A:
    pass

a = A()
setattr(a, S('x'), 1)
print(getattr(a, S('x')))
print(hasattr(a, S('x')), hasattr(a, S('y')))
name = S('attr_' + 'built_at_runtime')
setattr(a, name, 2)
print(getattr(a, name))
