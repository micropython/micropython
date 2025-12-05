#!/usr/bin/env python3
"""Debug the failing tests."""

# Test type.__init__ access
print("Testing type.__init__ access:")
print(f"hasattr(type, '__init__'): {hasattr(type, '__init__')}")


class CustomMeta(type):
    pass


print(f"hasattr(CustomMeta, '__init__'): {hasattr(CustomMeta, '__init__')}")


class RegularClass:
    pass


print(f"hasattr(RegularClass, '__init__'): {hasattr(RegularClass, '__init__')}")

# Test deep hierarchy
print("\nTesting deep hierarchy:")
init_order = []


class Meta1(type):
    def __init__(cls, name, bases, attrs):
        print(f"Meta1.__init__ called for {name}")
        super().__init__(name, bases, attrs)
        init_order.append("Meta1")


class Meta2(Meta1):
    def __init__(cls, name, bases, attrs):
        print(f"Meta2.__init__ called for {name}")
        super().__init__(name, bases, attrs)
        init_order.append("Meta2")


try:

    class Test(metaclass=Meta2):
        pass

    print(f"init_order: {init_order}")
except Exception as e:
    print(f"Error creating Test: {e}")
