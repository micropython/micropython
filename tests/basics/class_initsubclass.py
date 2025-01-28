# skip failing on minimal port
try:

    class T:
        pass

    T().__class__
    getattr

    def print_check_all_attrs(obj, name: str, attrs: "list[str]"):
        for attr in attrs:
            print(name, attr, getattr(obj, attr, "missing"))

except AttributeError:
    print("SKIP")
    raise SystemExit


class A:
    a = "A"

    def __init_subclass__(cls):
        print("A init_subclass", cls.__name__)
        cls.aa = "AA"

    print("class A")


class B(A):
    b = "B"
    print("class B")


print_check_all_attrs(A, "A", ["a", "aa", "b"])
print_check_all_attrs(B, "B", ["a", "aa", "b"])
