# Test 1: Basic metaclass __init__ call
init_called = []

class Meta(type):
    def __init__(cls, name, bases, attrs):
        super().__init__(name, bases, attrs)
        init_called.append(name)

class Test(metaclass=Meta):
    pass

assert 'Test' in init_called
print("Test 1: PASS - metaclass __init__ called")

# Test 2: Metaclass __init__ can modify class
class CounterMeta(type):
    def __init__(cls, name, bases, attrs):
        super().__init__(name, bases, attrs)
        cls.instance_count = 0

class Widget(metaclass=CounterMeta):
    def __init__(self):
        Widget.instance_count += 1

assert hasattr(Widget, 'instance_count')
assert Widget.instance_count == 0
w1 = Widget()
assert Widget.instance_count == 1
w2 = Widget()
assert Widget.instance_count == 2
print("Test 2: PASS - metaclass __init__ can modify class")

# Test 3: Arguments passed correctly
received_args = []

class ArgTrackerMeta(type):
    def __init__(cls, name, bases, attrs):
        super().__init__(name, bases, attrs)
        received_args.append({
            'name': name,
            'bases': bases,
            'has_method': 'foo' in attrs
        })

class TestClass(metaclass=ArgTrackerMeta):
    def foo(self):
        pass

assert len(received_args) == 1
assert received_args[0]['name'] == 'TestClass'
assert received_args[0]['has_method'] == True
print("Test 3: PASS - correct arguments passed to __init__")

# Test 4: Inherited metaclass __init__
inherited_calls = []

class BaseMeta(type):
    def __init__(cls, name, bases, attrs):
        super().__init__(name, bases, attrs)
        inherited_calls.append(name)

class DerivedMeta(BaseMeta):
    pass  # No __init__ in this class's dict

class InheritedTest(metaclass=DerivedMeta):
    pass

assert 'InheritedTest' in inherited_calls
print("Test 4: PASS - inherited metaclass __init__ called")

# Test 5: Invalid metaclass __init__ signature raises TypeError
class BadMeta(type):
    def __init__(cls):  # Wrong signature - only takes 1 arg instead of 4
        pass

try:
    class Bad(metaclass=BadMeta):
        pass
    print("Test 5: FAIL - Should have raised TypeError")
except TypeError:
    print("Test 5: PASS - TypeError raised for bad signature")

print("\nAll metaclass __init__ tests passed!")
