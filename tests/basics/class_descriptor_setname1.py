
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


# Test mutations of owner class.


class SetSibling:
    def __set_name__(self, owner, name):
        setattr(owner, name + "_sib", 121)


try:
    class SetSiblingTest:
        desc = SetSibling()
        
except AttributeError:
    print("AttributeError")
else:
    t120 = SetSiblingTest()
    print(t120.desc_sib)


class DelSibling:
    def __set_name__(self, owner, name):
        delattr(owner, name + "_sib")


try:
    class DelSiblingTest:
        desc = DelSibling()
        desc_sib = 131

except AttributeError:
    print("AttributeError")
else:
    t130 = DelSiblingTest()

    try:
        print(t130.desc_sib)
    except AttributeError:
        print("(deleted)")



class SetSelf:
    def __set_name__(self, owner, name):
        setattr(owner, name, 221)


try:
    class SetSelfTest:
        desc = SetSelf()

except AttributeError:
    print("AttributeError")
else:
    t220 = SetSelfTest()
    print(t220.desc)


class DelSelf:
    def __set_name__(self, owner, name):
        delattr(owner, name)


try:
    class DelSelfTest:
        desc = DelSelf()

except AttributeError:
    print("AttributeError")
else:
    t230 = DelSelfTest()

    try:
        print(t230.desc)
    except AttributeError:
        print("(deleted)")
