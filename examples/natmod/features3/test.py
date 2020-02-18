from features3 import Native

class Derived(Native):
    def foo(self, val):
        print('Derived.foo', val)

class Derived2(Derived):
    def foo(self, val):
        print('Derived2.write', val)

def test():
    # Test single inheritance
    f = Derived()
    f.add(123)
    f.call_foo()

    # Call via bound method
    f.add(456)
    fun = f.call_foo
    fun()

    # Test double inheritance
    f = Derived2()
    f.add(123)
    f.call_foo()

    # Call via bound method
    f.add(456)
    fun = f.call_foo
    fun()

test()
