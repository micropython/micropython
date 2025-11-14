# Test metaclass __prepare__ method (PEP 3115)

# Skip test if __prepare__ is not supported
_prepare_test = []
class _TestMeta(type):
    @classmethod
    def __prepare__(mcs, name, bases):
        _prepare_test.append(1)
        return {}

class _Test(metaclass=_TestMeta):
    pass

if not _prepare_test:
    print("SKIP")
    raise SystemExit

# Test 1: Basic __prepare__ is called before class body execution
print("Test 1: Basic __prepare__ call")
prepare_log = []

class Meta1(type):
    @classmethod
    def __prepare__(mcs, name, bases):
        prepare_log.append(f"__prepare__({name})")
        return {}

class Test1(metaclass=Meta1):
    prepare_log.append("body")

print(prepare_log)
print("PASS" if prepare_log == ["__prepare__(Test1)", "body"] else "FAIL")

# Test 2: __prepare__ receives correct arguments
print("\nTest 2: __prepare__ arguments")

class Meta2(type):
    @classmethod
    def __prepare__(mcs, name, bases):
        print(f"mcs={mcs.__name__}, name={name}, bases={bases}")
        return {}

class Base2:
    pass

class Test2(Base2, metaclass=Meta2):
    pass

# Test 3: __prepare__ return value is used as class namespace
print("\nTest 3: __prepare__ return value as namespace")

class Meta3(type):
    @classmethod
    def __prepare__(mcs, name, bases):
        # Pre-populate namespace with a value
        return {"injected": 42}

class Test3(metaclass=Meta3):
    pass

print(f"Test3.injected = {Test3.injected}")
print("PASS" if Test3.injected == 42 else "FAIL")

# Test 4: __prepare__ can access namespace in __new__
print("\nTest 4: Access namespace from __new__")

class Meta4(type):
    @classmethod
    def __prepare__(mcs, name, bases):
        # Return dict with tracking info
        d = {}
        d['_prepared'] = True
        return d

    def __new__(mcs, name, bases, namespace):
        # Verify __prepare__ returned dict was used
        was_prepared = namespace.get('_prepared', False)
        cls = type.__new__(mcs, name, bases, dict(namespace))
        cls._was_prepared = was_prepared
        return cls

class Test4(metaclass=Meta4):
    x = 1

print(f"Was prepared: {Test4._was_prepared}")
print("PASS" if Test4._was_prepared else "FAIL")

# Test 5: __prepare__ inheritance
print("\nTest 5: Inherited __prepare__")

class BaseMeta(type):
    @classmethod
    def __prepare__(mcs, name, bases):
        print(f"BaseMeta.__prepare__({name})")
        return {}

class DerivedMeta(BaseMeta):
    pass

class Test5(metaclass=DerivedMeta):
    pass

# Test 6: __prepare__ not called when metaclass doesn't define it
print("\nTest 6: No __prepare__ defined")

class Meta6(type):
    pass

class Test6(metaclass=Meta6):
    x = 1

print(f"Test6.x = {Test6.x}")
print("PASS")

print("\nAll __prepare__ tests completed!")
