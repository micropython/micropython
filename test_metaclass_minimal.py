#!/usr/bin/env python3
"""Minimal edge case tests for the metaclass __init__ fix."""

import sys


def test_case(name, func):
    """Helper to run test cases and catch errors."""
    try:
        func()
        print(f"{name}: PASS")
        return True
    except Exception as e:
        print(f"{name}: FAIL - {e}")
        return False


# Test 1: The original crash scenario
def test_basic_metaclass_super():
    """Basic super().__init__() in metaclass - should NOT crash."""

    class Meta(type):
        def __init__(cls, name, bases, attrs):
            super().__init__(name, bases, attrs)

    class Test(metaclass=Meta):
        pass

    assert Test.__name__ == "Test"


# Test 2: type.__init__ signature verification
def test_type_init_signature():
    """Verify type.__init__ accepts correct arguments."""
    # 1 argument should work
    type.__init__(type)

    # 4 arguments should work
    class Meta(type):
        pass

    type.__init__(Meta, "Test", (), {})

    # Wrong number of arguments should fail
    try:
        type.__init__(type, "extra")
        assert False, "Should have raised TypeError"
    except TypeError:
        pass  # Expected


# Test 3: type.__init__ is accessible only where appropriate
def test_type_init_access():
    """Test type.__init__ accessibility."""
    # type itself should have __init__
    assert hasattr(type, "__init__")

    # Custom metaclass should have access to type.__init__
    class CustomMeta(type):
        pass

    assert hasattr(CustomMeta, "__init__")

    # Regular class behavior (implementation-specific)
    class RegularClass:
        pass

    # Just verify it doesn't crash
    _ = getattr(RegularClass, "__init__", None)


# Test 4: super().__init__() in metaclass doesn't create duplicate type
def test_no_duplicate_type_creation():
    """Ensure super().__init__() doesn't create a new type object."""
    created_types = []

    class Meta(type):
        def __init__(cls, name, bases, attrs):
            created_types.append(cls)
            super().__init__(name, bases, attrs)
            created_types.append(cls)

    class Test(metaclass=Meta):
        pass

    # Should be the same object before and after super().__init__()
    assert len(created_types) == 2
    assert created_types[0] is created_types[1]
    assert created_types[0] is Test


# Test 5: Deep metaclass hierarchy works
def test_deep_hierarchy():
    """Test nested metaclass inheritance."""
    init_order = []

    class Meta1(type):
        def __init__(cls, name, bases, attrs):
            super().__init__(name, bases, attrs)
            init_order.append("Meta1")

    class Meta2(Meta1):
        def __init__(cls, name, bases, attrs):
            super().__init__(name, bases, attrs)
            init_order.append("Meta2")

    class Test(metaclass=Meta2):
        pass

    # Meta2's __init__ should run after Meta1's (due to super() order)
    assert "Meta1" in init_order
    assert "Meta2" in init_order


# Test 6: type.__init__ is truly a no-op
def test_type_init_noop():
    """Verify type.__init__ doesn't modify the class."""

    class Meta(type):
        pass

    # Create a class with specific attributes
    Test = Meta("Test", (), {"x": 42})
    original_name = Test.__name__
    original_x = Test.x

    # Call type.__init__ with different arguments
    type.__init__(Test, "Different", (), {"y": 99})

    # Nothing should have changed
    assert Test.__name__ == original_name
    assert Test.x == original_x
    assert not hasattr(Test, "y")


# Test 7: Instance super().__init__() still works correctly
def test_instance_init():
    """Verify instance __init__ wasn't broken by the fix."""
    init_log = []

    class Base:
        def __init__(self):
            init_log.append("Base")

    class Derived(Base):
        def __init__(self):
            super().__init__()
            init_log.append("Derived")

    obj = Derived()
    assert init_log == ["Base", "Derived"]
    assert isinstance(obj, Derived)


# Run all tests
if __name__ == "__main__":
    is_micropython = sys.implementation.name == "micropython"
    print(f"Running tests on {sys.implementation.name}")
    print("=" * 50)

    tests = [
        ("Basic metaclass super().__init__", test_basic_metaclass_super),
        ("type.__init__ signature", test_type_init_signature),
        ("type.__init__ access", test_type_init_access),
        ("No duplicate type creation", test_no_duplicate_type_creation),
        ("Deep hierarchy", test_deep_hierarchy),
        ("type.__init__ is no-op", test_type_init_noop),
        ("Instance __init__ works", test_instance_init),
    ]

    passed = 0
    failed = 0

    for name, test_func in tests:
        if test_case(name, test_func):
            passed += 1
        else:
            failed += 1

    print("=" * 50)
    print(f"Results: {passed} passed, {failed} failed")
    if failed == 0:
        print("All tests PASSED!")
    else:
        print("FAILED tests detected!")
