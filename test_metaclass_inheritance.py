"""Test metaclass __init__ inheritance"""

# Test inherited metaclass __init__
init_calls = []


class BaseMeta(type):
    def __init__(cls, name, bases, attrs):
        super().__init__(name, bases, attrs)
        init_calls.append(f"BaseMeta.__init__({name})")
        print(f"BaseMeta.__init__({name})")


class DerivedMeta(BaseMeta):
    pass  # No __init__ in this class's dict


class TestClass(metaclass=DerivedMeta):
    pass


print(f"\nInit calls: {init_calls}")
print("Expected: ['BaseMeta.__init__(TestClass)']")
print(f"Pass: {init_calls == ['BaseMeta.__init__(TestClass)']}")
