# Test 1: Metaclass property access
class Meta(type):
    @property
    def class_id(cls):
        return cls.__name__ + "_id"

class Widget(metaclass=Meta):
    pass

assert Widget.class_id == "Widget_id"
print("Test 1: PASS - metaclass property access")

# Test 2: Metaclass method access
class Meta2(type):
    def get_name(cls):
        return cls.__name__

    def combine(cls, *parts):
        return cls.__name__ + ":" + ":".join(parts)

class Gadget(metaclass=Meta2):
    pass

assert Gadget.get_name() == "Gadget"
assert Gadget.combine('a', 'b', 'c') == "Gadget:a:b:c"
print("Test 2: PASS - metaclass method access")

# Test 3: Property with computed value
class DataMeta(type):
    @property
    def data_type(cls):
        # Dynamically compute based on class attributes
        if hasattr(cls, '_is_string'):
            return "string"
        elif hasattr(cls, '_is_number'):
            return "number"
        else:
            return "unknown"

class StringData(metaclass=DataMeta):
    _is_string = True

class NumberData(metaclass=DataMeta):
    _is_number = True

class GenericData(metaclass=DataMeta):
    pass

assert StringData.data_type == "string"
assert NumberData.data_type == "number"
assert GenericData.data_type == "unknown"
print("Test 3: PASS - property with computed value")

# Test 4: Multiple metaclass levels
class BaseMeta(type):
    @property
    def level(cls):
        return "base"

class DerivedMeta(BaseMeta):
    @property
    def derived_level(cls):
        return "derived"

class MyClass(metaclass=DerivedMeta):
    pass

assert MyClass.level == "base"
assert MyClass.derived_level == "derived"
print("Test 4: PASS - multiple metaclass levels")

print("\nAll metaclass property tests passed!")
