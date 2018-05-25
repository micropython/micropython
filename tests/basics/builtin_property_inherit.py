# test builtin property combined with inheritance
try:
    property
except:
    print("SKIP")
    raise SystemExit

# test property in a base class works for derived classes
class A:
    @property
    def x(self):
        print('A x')
        return 123
class B(A):
    pass
class C(B):
    pass
class D:
    pass
class E(C, D):
    pass
print(A().x)
print(B().x)
print(C().x)
print(E().x)

# test that we can add a property to base class after creation
class F:
    pass
F.foo = property(lambda self: print('foo get'))
class G(F):
    pass
F().foo
G().foo

# should be able to add a property to already-subclassed class because it already has one
F.bar = property(lambda self: print('bar get'))
F().bar
G().bar

# test case where class (H here) is already subclassed before adding attributes
class H:
    pass
class I(H):
    pass

# should be able to add a normal member to already-subclassed class
H.val = 2
print(I().val)

# should be able to add a property to the derived class
I.baz = property(lambda self: print('baz get'))
I().baz
