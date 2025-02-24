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


# Test basic hazard-free mutations of the enclosing class.
# See also cpydiff/core_class_setname_hazard.py for broken non-hazard-free cases.


class GetSibling:
    def __set_name__(self, owner, name):
        print(getattr(owner, name + "_sib"))


class GetSiblingTest:
    desc = GetSibling()
    desc_sib = 101


t101 = GetSiblingTest()


class SetSibling:
    def __set_name__(self, owner, name):
        setattr(owner, name + "_sib", 102)


class SetSiblingTest:
    desc = SetSibling()


t102 = SetSiblingTest()

print(t102.desc_sib)


class DelSibling:
    def __set_name__(self, owner, name):
        delattr(owner, name + "_sib")


class DelSiblingTest:
    desc = DelSibling()
    desc_sib = 103


t103 = DelSiblingTest()

try:
    print(t103.desc_sib)
except AttributeError:
    print("AttributeError")


class GetSelf:
    x = 201

    def __set_name__(self, owner, name):
        print(getattr(owner, name).x)


class GetSelfTest:
    desc = GetSelf()


t201 = GetSelfTest()


class SetSelf:
    def __set_name__(self, owner, name):
        setattr(owner, name, 202)


class SetSelfTest:
    desc = SetSelf()


t202 = SetSelfTest()

print(t202.desc)


class DelSelf:
    def __set_name__(self, owner, name):
        delattr(owner, name)


class DelSelfTest:
    desc = DelSelf()


t203 = DelSelfTest()

try:
    print(t203.desc)
except AttributeError:
    print("AttributeError")
