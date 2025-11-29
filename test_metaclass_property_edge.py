"""Test metaclass property edge cases"""


# Test 1: Property with no getter
class Meta1(type):
    # Create property with no getter
    def _setter(self, v):
        pass

    no_getter = property(None, _setter)


class Test1(metaclass=Meta1):
    pass


try:
    x = Test1.no_getter
    print("Test 1 FAIL: Should have raised AttributeError")
except AttributeError:
    print("Test 1 PASS: AttributeError raised for property with no getter")


# Test 2: Property that raises exception
class Meta2(type):
    def _prop_getter(cls):
        raise ValueError("Property getter failed")

    raising_prop = property(_prop_getter)


class Test2(metaclass=Meta2):
    pass


try:
    x = Test2.raising_prop
    print("Test 2 FAIL: Should have raised ValueError")
except ValueError as e:
    print("Test 2 PASS: ValueError propagated:", str(e))


# Test 3: Inherited metaclass property
class BaseMeta(type):
    def _inherited_getter(cls):
        return "inherited from " + cls.__name__

    inherited_prop = property(_inherited_getter)


class DerivedMeta(BaseMeta):
    pass


class Test3(metaclass=DerivedMeta):
    pass


result = Test3.inherited_prop
print("Test 3:", result)
expected = "inherited from Test3"
print("Test 3 PASS:", result == expected)


# Test 4: Method on metaclass
class Meta4(type):
    def my_method(cls, arg):
        return cls.__name__ + ": " + arg


class Test4(metaclass=Meta4):
    pass


result = Test4.my_method("hello")
print("Test 4:", result)
expected4 = "Test4: hello"
print("Test 4 PASS:", result == expected4)
