"""Basic test of __prepare__ metaclass support"""

prepare_called = []


class Meta(type):
    @classmethod
    def __prepare__(mcs, name, bases):
        print(f"__prepare__ called for {name}")
        prepare_called.append(name)
        return {}


class Test(metaclass=Meta):
    x = 1
    y = 2


print(f"prepare_called: {prepare_called}")
assert "Test" in prepare_called, "FAIL: __prepare__ was not called"
print("PASS: __prepare__ functionality works!")
