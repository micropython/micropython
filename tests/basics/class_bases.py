# test for type.__bases__ implementation

if not hasattr(object, '__bases__'):
    print("SKIP")
    raise SystemExit

class A:
    pass

class B(object):
    pass

class C(B):
    pass

class D(C, A):
    pass

# Check the attribute exists
print(hasattr(A, '__bases__'))
print(hasattr(B, '__bases__'))
print(hasattr(C, '__bases__'))
print(hasattr(D, '__bases__'))

# Check it is always a tuple
print(type(A.__bases__) == tuple)
print(type(B.__bases__) == tuple)
print(type(C.__bases__) == tuple)
print(type(D.__bases__) == tuple)

# Check size
print(len(A.__bases__) == 1)
print(len(B.__bases__) == 1)
print(len(C.__bases__) == 1)
print(len(D.__bases__) == 2)

# Check values
print(A.__bases__[0] == object)
print(B.__bases__[0] == object)
print(C.__bases__[0] == B)
print(D.__bases__[0] == C)
print(D.__bases__[1] == A)

# Object has an empty tuple
print(object.__bases__ == tuple())
