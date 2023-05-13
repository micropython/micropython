print("Now comes the language constructions tests.")


# function
def test_func():
    def test_sub_func():
        print("test_function")

    test_sub_func()


# closure
def test_closure(msg):
    def make_closure():
        print(msg)

    return make_closure


# exception
def test_exception():
    try:
        raise Exception("test_exception")

    except Exception:
        pass

    finally:
        pass


# listcomp
def test_listcomp():
    print("test_listcomp", [x for x in range(3)])


# lambda
def test_lambda():
    func_obj_1 = lambda a, b: a + b
    print(func_obj_1(10, 20))


# import
def test_import():
    from sys_settrace_subdir import sys_settrace_importme

    sys_settrace_importme.dummy()
    sys_settrace_importme.saysomething()


# class
class TLClass:
    def method():
        pass

    pass


def test_class():
    class TestClass:
        __anynum = -9

        def method(self):
            print("test_class_method")
            self.__anynum += 1

        def prprty_getter(self):
            return self.__anynum

        def prprty_setter(self, what):
            self.__anynum = what

        prprty = property(prprty_getter, prprty_setter)

    cls = TestClass()
    cls.method()
    print("test_class_property", cls.prprty)
    cls.prprty = 12
    print("test_class_property", cls.prprty)


def run_tests():
    test_func()
    test_closure_inst = test_closure("test_closure")
    test_closure_inst()
    test_exception()
    test_listcomp()
    test_lambda()
    test_class()
    test_import()


print("And it's done!")
