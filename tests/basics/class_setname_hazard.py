# Test that __set_name__ can access and mutate its owner argument.


def skip_if_no_descriptors():
    class Descriptor:
        def __get__(self, obj, cls):
            return

    class TestClass:
        Forward = Descriptor()

    a = TestClass()
    try:
        a.__class__
    except AttributeError:
        # Target doesn't support __class__.
        print("SKIP")
        raise SystemExit

    b = a.Forward
    if "Descriptor" in repr(b.__class__):
        # Target doesn't support descriptors.
        print("SKIP")
        raise SystemExit


skip_if_no_descriptors()


# Test basic accesses and mutations.


class GetSibling:
    def __set_name__(self, owner, name):
        print(getattr(owner, name + "_sib"))


class GetSiblingTest:
    desc = GetSibling()
    desc_sib = 111


t110 = GetSiblingTest()


class SetSibling:
    def __set_name__(self, owner, name):
        setattr(owner, name + "_sib", 121)


class SetSiblingTest:
    desc = SetSibling()


t120 = SetSiblingTest()

print(t120.desc_sib)


class DelSibling:
    def __set_name__(self, owner, name):
        delattr(owner, name + "_sib")


class DelSiblingTest:
    desc = DelSibling()
    desc_sib = 131


t130 = DelSiblingTest()

try:
    print(t130.desc_sib)
except AttributeError:
    print("AttributeError")


class GetSelf:
    x = 211

    def __set_name__(self, owner, name):
        print(getattr(owner, name).x)


class GetSelfTest:
    desc = GetSelf()


t210 = GetSelfTest()


class SetSelf:
    def __set_name__(self, owner, name):
        setattr(owner, name, 221)


class SetSelfTest:
    desc = SetSelf()


t220 = SetSelfTest()

print(t220.desc)


class DelSelf:
    def __set_name__(self, owner, name):
        delattr(owner, name)


class DelSelfTest:
    desc = DelSelf()


t230 = DelSelfTest()

try:
    print(t230.desc)
except AttributeError:
    print("AttributeError")


# Test exception behavior.


class Raise:
    def __set_name__(self, owner, name):
        raise Exception()


try:

    class RaiseTest:
        desc = Raise()
except Exception as e:  # CPython raises RuntimeError, MicroPython propagates the original exception
    print("Exception")


# Ensure removed/overwritten class members still get __set_name__ called.


class SetSpecific:
    def __init__(self, sib_name, sib_replace):
        self.sib_name = sib_name
        self.sib_replace = sib_replace

    def __set_name__(self, owner, name):
        setattr(owner, self.sib_name, self.sib_replace)


class SetReplaceTest:
    a = SetSpecific("b", 312)  # one of these is changed first
    b = SetSpecific("a", 311)


t310 = SetReplaceTest()
print(t310.a)
print(t310.b)


class DelSpecific:
    def __init__(self, sib_name):
        self.sib_name = sib_name

    def __set_name__(self, owner, name):
        delattr(owner, self.sib_name)


class DelReplaceTest:
    a = DelSpecific("b")  # one of these is removed first
    b = DelSpecific("a")


t320 = DelReplaceTest()
try:
    print(t320.a)
except AttributeError:
    print("AttributeError")
try:
    print(t320.b)
except AttributeError:
    print("AttributeError")
