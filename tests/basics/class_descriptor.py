class Descriptor:
    def __get__(self, obj, cls):
        print("get")
        print(type(obj) is Main)
        print(cls is Main)
        return "result"

    def __set__(self, obj, val):
        print("set")
        print(type(obj) is Main)
        print(val)

    def __delete__(self, obj):
        print("delete")
        print(type(obj) is Main)

    def __set_name__(self, owner, name):
        print("set_name", name)
        print(owner.__name__ == "Main")


class Main:
    Forward = Descriptor()


m = Main()
try:
    m.__class__
except AttributeError:
    # Target doesn't support __class__.
    print("SKIP")
    raise SystemExit

r = m.Forward
if "Descriptor" in repr(r.__class__):
    # Target doesn't support descriptors.
    print("SKIP")
    raise SystemExit

# Test assignment and deletion.

print(r)
m.Forward = "a"
del m.Forward

# Test that lookup of descriptors like __get__ are not passed into __getattr__.


class NonDescriptor:
    def __getattr__(self, attr):
        print("getattr", attr)


class TestClass:
    non_descriptor = NonDescriptor()


print(isinstance(TestClass().non_descriptor, NonDescriptor))

t = TestClass()
t.non_descriptor = 123
print(t.non_descriptor)

try:
    del TestClass().non_descriptor
except AttributeError:
    print("AttributeError")

# Test that super() applies the descriptor protocol to attributes found in a
# base class.


class SuperDescriptor:
    def __get__(self, obj, cls):
        print("super get")
        print(type(obj) is cls)
        return "super result"


class SuperBase:
    attr = SuperDescriptor()


class SuperSub(SuperBase):
    def get(self):
        return super().attr


print(SuperSub().get())

# A class attribute that shadows the descriptor in the subclass does not
# change what super() finds.


class SuperShadow(SuperBase):
    attr = "shadow"

    def get(self):
        return super().attr


print(SuperShadow().get())

# Likewise for a method that shadows the descriptor.


class SuperMethod(SuperBase):
    def attr(self):
        return super().attr


print(SuperMethod().attr())
