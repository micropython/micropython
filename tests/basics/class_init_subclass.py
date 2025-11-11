# Test __init_subclass__ functionality (PEP 487)

# Check if __init_subclass__ support is enabled
try:
    class _TestBase:
        def __init_subclass__(cls):
            cls._called = True
    
    class _TestDerived(_TestBase):
        pass
    
    if not hasattr(_TestDerived, '_called'):
        raise TypeError("__init_subclass__ not called")
except (TypeError, AttributeError):
    print("SKIP")
    raise SystemExit

# Test 1: __init_subclass__ WITHOUT @classmethod decorator (implicit classmethod per PEP 487)
print("Test 1: __init_subclass__ without decorator")
class Base:
    def __init_subclass__(cls):
        print("__init_subclass__ called for cls")
        cls.from_init_subclass = True

class Derived(Base):
    pass

print("Derived.from_init_subclass:", Derived.from_init_subclass)

# Test 2: __init_subclass__ with multiple parameters
print("\nTest 2: __init_subclass__ with multiple parameters")
class Base2:
    def __init_subclass__(cls, *args, **kwargs):
        print("__init_subclass__ (with *args, **kwargs) for cls")
        print("  args:", args)
        print("  kwargs:", kwargs)
        cls.marker = "has_args"

class Derived2(Base2):
    pass

print("Derived2.marker:", Derived2.marker)

# Test 3: __init_subclass__ with keyword arguments (no decorator needed)
print("\nTest 3: __init_subclass__ with kwargs")
class Base3:
    def __init_subclass__(cls, **kwargs):
        print("__init_subclass__ for", cls, "with kwargs:", kwargs)
        cls.kwargs = kwargs

class Derived3(Base3, foo='bar', init=True):
    pass

print("Derived3.kwargs:", Derived3.kwargs)

# Test 4: __init_subclass__ with named kwargs (dataclass pattern, no decorator)
print("\nTest 4: Dataclass-style __init_subclass__")
class ModelBase:
    def __init_subclass__(cls, *, init=True, frozen=False, eq=True):
        print("ModelBase.__init_subclass__ for", cls)
        print("  init={}, frozen={}, eq={}".format(init, frozen, eq))
        cls.config = {'init': init, 'frozen': frozen, 'eq': eq}

class CustomerModel(ModelBase, init=False, frozen=True, eq=False):
    pass

print("CustomerModel.config:", CustomerModel.config)

# Test 5: Multiple levels of inheritance (only immediate parent's __init_subclass__ called)
print("\nTest 5: Multiple levels of inheritance")
class GrandParent:
    def __init_subclass__(cls):
        print("GrandParent.__init_subclass__ for", cls)
        cls.from_grandparent = True

class Parent(GrandParent):
    def __init_subclass__(cls):
        print("Parent.__init_subclass__ for", cls)
        cls.from_parent = True

class Child(Parent):
    pass

# Only Parent's __init_subclass__ is called when Child is created
# But Child inherits from_grandparent from Parent (which got it when Parent was created)
print("Child.from_parent:", Child.from_parent)
print("Child.from_grandparent:", Child.from_grandparent)

# Test 6: Multiple inheritance (only first base's __init_subclass__ called per PEP 487)
print("\nTest 6: Multiple inheritance")
class BaseA:
    def __init_subclass__(cls, **kwargs):
        print("BaseA.__init_subclass__ for", cls)
        cls.from_a = True

class BaseB:
    def __init_subclass__(cls, **kwargs):
        print("BaseB.__init_subclass__ for", cls)
        cls.from_b = True

class Multi(BaseA, BaseB):
    pass

# Per PEP 487, only BaseA's __init_subclass__ is called (first in MRO)
# BaseB's is NOT called unless BaseA chains via super()
print("Multi.from_a:", Multi.from_a)
try:
    print("Multi.from_b:", Multi.from_b)
except AttributeError:
    print("Multi.from_b: AttributeError (as expected - only first base called)")

# Test 7: Base class without __init_subclass__ (should not error)
print("\nTest 7: Base without __init_subclass__")
class NoInit:
    pass

class DerivedNoInit(NoInit):
    pass

print("DerivedNoInit created successfully")

print("\nAll tests passed!")
