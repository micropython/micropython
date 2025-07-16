
set_name_called = False

class Descriptor:
    def __set_name__(self, owner, name):
        global set_name_called
        set_name_called = True

class TestClass:
    Forward = Descriptor()

if not set_name_called:
    print("SKIP")
    raise SystemExit


# Make sure classes get unfrozen after the __set_name__ phase

try:
    TestClass.x = 5
    print(TestClass.x)
except AttributeError:
    print("AttributeError")


# Test accesses to owner class.


class GetSibling:
    def __set_name__(self, owner, name):
        print(getattr(owner, name + "_sib"))


class GetSiblingTest:
    desc = GetSibling()
    desc_sib = 111


t110 = GetSiblingTest()


class GetSelf:
    x = 211

    def __set_name__(self, owner, name):
        print(getattr(owner, name).x)


class GetSelfTest:
    desc = GetSelf()


t210 = GetSelfTest()


# Test exception behavior


class Raise:
    def __set_name__(self, owner, name):
        raise Exception()


try:
    class RaiseTest:
        desc = Raise()
except Exception as e:
    print("Exception")
