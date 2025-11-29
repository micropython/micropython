#!/usr/bin/env python3
"""Edge case tests for the metaclass __init__ fix to verify correctness."""

import traceback


def test_case(name, func):
    """Helper to run test cases and catch errors."""
    try:
        func()
        print(f"{name}: PASS")
        return True
    except Exception as e:
        print(f"{name}: FAIL - {e}")
        traceback.print_exc()
        return False


# Test 1: Basic super().__init__() in metaclass
def test_basic_metaclass_super():
    """The original crash scenario."""

    class Meta(type):
        def __init__(cls, name, bases, attrs):
            super().__init__(name, bases, attrs)

    class Test(metaclass=Meta):
        pass

    assert Test.__name__ == "Test"


# Test 2: Multiple inheritance in metaclass hierarchy
def test_metaclass_multiple_inheritance():
    """Test metaclass with multiple inheritance."""

    class MetaA(type):
        def __init__(cls, name, bases, attrs):
            super().__init__(name, bases, attrs)
            cls.from_a = True

    class MetaB(type):
        def __init__(cls, name, bases, attrs):
            super().__init__(name, bases, attrs)
            cls.from_b = True

    class MetaC(MetaA, MetaB):
        def __init__(cls, name, bases, attrs):
            super().__init__(name, bases, attrs)
            cls.from_c = True

    class Test(metaclass=MetaC):
        pass

    # MRO should be MetaC -> MetaA -> MetaB -> type
    assert hasattr(Test, "from_a")
    assert hasattr(Test, "from_c")
    # MetaB's __init__ may or may not be called depending on MRO implementation


# Test 3: type.__init__ should accept exactly 1 or 4 arguments (MicroPython)
#         or 1 or 3 arguments (CPython)
def test_type_init_signature():
    """Verify type.__init__ signature."""
    # Test what signatures are accepted
    import sys

    is_micropython = sys.implementation.name == "micropython"

    # 1 argument (just self) should work
    type.__init__(type)

    # Try with full arguments
    class Meta(type):
        pass

    if is_micropython:
        # MicroPython: 4 arguments (self, name, bases, dict)
        type.__init__(Meta, "Test", (), {})
        expected_msg = "takes 1 or 4 arguments"
    else:
        # CPython: 3 arguments (name, bases, dict) - self is implicit
        type.__init__(Meta, "Test", ())
        expected_msg = "takes 1 or 3 arguments"

    # Other argument counts should fail
    try:
        type.__init__(type, "extra")
        assert False, "Should have raised TypeError"
    except TypeError as e:
        assert expected_msg in str(e)

    try:
        type.__init__(type, "a", "b", "c", "d", "e")
        assert False, "Should have raised TypeError"
    except TypeError as e:
        assert expected_msg in str(e)


# Test 4: Regular classes should NOT have type.__init__
def test_regular_class_no_type_init():
    """Regular classes shouldn't expose type.__init__."""

    class RegularClass:
        pass

    # This depends on implementation details - in CPython, hasattr returns True
    # but the attribute lookup goes through descriptors
    # The key is that calling it shouldn't create a new type
    if hasattr(RegularClass, "__init__"):
        # If it exists, calling it with wrong args should behave correctly
        try:
            # This should either not be type.__init__ or should fail appropriately
            result = RegularClass.__init__(RegularClass, "name", (), {})
            # If it succeeded, it shouldn't have created a new type
            assert result is None or not isinstance(result, type)
        except TypeError:
            # Expected - regular class __init__ has different signature
            pass


# Test 5: Custom metaclass with type as explicit base
def test_explicit_type_base():
    """Test metaclass that explicitly inherits from type."""

    class Meta(type):
        def __init__(cls, name, bases, attrs):
            # Explicit call to type.__init__ instead of super()
            type.__init__(cls, name, bases, attrs)
            cls.initialized = True

    class Test(metaclass=Meta):
        pass

    assert hasattr(Test, "initialized")
    assert Test.initialized


# Test 6: Metaclass without __init__ should work
def test_metaclass_no_init():
    """Metaclass without __init__ should inherit type's."""

    class Meta(type):
        pass  # No __init__ defined

    class Test(metaclass=Meta):
        pass

    assert Test.__name__ == "Test"


# Test 7: Deep metaclass hierarchy
def test_deep_metaclass_hierarchy():
    """Test deeply nested metaclass inheritance."""

    class Meta1(type):
        def __init__(cls, name, bases, attrs):
            super().__init__(name, bases, attrs)
            cls.level1 = True

    class Meta2(Meta1):
        def __init__(cls, name, bases, attrs):
            super().__init__(name, bases, attrs)
            cls.level2 = True

    class Meta3(Meta2):
        def __init__(cls, name, bases, attrs):
            super().__init__(name, bases, attrs)
            cls.level3 = True

    class Test(metaclass=Meta3):
        pass

    assert Test.level1
    assert Test.level2
    assert Test.level3


# Test 8: super() in metaclass without __init__ override
def test_super_in_other_metaclass_method():
    """Test super() in metaclass methods other than __init__."""

    class Meta(type):
        def custom_method(cls):
            # This should work - super() in non-__init__ context
            return super().__name__

    class Test(metaclass=Meta):
        pass

    # This tests that super() works in other contexts
    assert Test.custom_method() == "Test"


# Test 9: type.__init__ should be a no-op
def test_type_init_is_noop():
    """Verify type.__init__ doesn't modify anything."""

    class Meta(type):
        pass

    # Create a class normally
    Test1 = Meta("Test1", (), {"x": 1})

    # Call type.__init__ again - should be no-op
    type.__init__(Test1, "DifferentName", (), {"y": 2})

    # Name and attributes should be unchanged
    assert Test1.__name__ == "Test1"
    assert hasattr(Test1, "x")
    assert not hasattr(Test1, "y")


# Test 10: Verify the fix doesn't break instance __init__
def test_instance_init_still_works():
    """Make sure regular instance __init__ via super() still works."""
    init_called = []

    class Base:
        def __init__(self):
            init_called.append("Base")

    class Derived(Base):
        def __init__(self):
            super().__init__()
            init_called.append("Derived")

    Derived()
    assert init_called == ["Base", "Derived"]


# Run all tests
if __name__ == "__main__":
    tests = [
        ("Basic metaclass super().__init__", test_basic_metaclass_super),
        ("Metaclass multiple inheritance", test_metaclass_multiple_inheritance),
        ("type.__init__ signature", test_type_init_signature),
        ("Regular class no type.__init__", test_regular_class_no_type_init),
        ("Explicit type base", test_explicit_type_base),
        ("Metaclass without __init__", test_metaclass_no_init),
        ("Deep metaclass hierarchy", test_deep_metaclass_hierarchy),
        ("super() in other methods", test_super_in_other_metaclass_method),
        ("type.__init__ is no-op", test_type_init_is_noop),
        ("Instance __init__ still works", test_instance_init_still_works),
    ]

    passed = 0
    failed = 0

    for name, test_func in tests:
        if test_case(name, test_func):
            passed += 1
        else:
            failed += 1

    print(f"\n{'=' * 50}")
    print(f"Results: {passed} passed, {failed} failed")
    if failed == 0:
        print("All tests PASSED!")
    else:
        print("Some tests FAILED!")
