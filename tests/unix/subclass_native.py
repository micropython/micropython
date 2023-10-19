try:
    NativeBaseClass
except NameError:
    print("SKIP")
    raise SystemExit

# This tests two things that CircuitPython uses:
# 1. Native class's `make_new` get kwargs.
# 2. Native class's properties get used instead of creating a new attribute on
#    the subclass instance.

n = NativeBaseClass(test="direct kwarg")
print(".test:", n.test)

n.test = "test set directly"
print(".test:", n.test)


class A(NativeBaseClass):
    pass


a = A(test="subclass kwarg")
print(".test:", a.test)

a.test = "test set indirectly"
print(".test:", a.test)

a._new_attribute = True
print("._new_attribute", a._new_attribute)


class B(NativeBaseClass):
    def __init__(self, suffix):
        super().__init__(test="super init " + suffix)


b = B("suffix")
print(".test:", b.test)

b.test = "test set indirectly through b"
print(".test:", b.test)

b._new_attribute = True
print("._new_attribute", b._new_attribute)
