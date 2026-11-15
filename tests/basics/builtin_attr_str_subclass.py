class S(str):
    pass


class A:
    pass


a = A()
a.value = 12
print(getattr(a, S("value")))
print(hasattr(a, S("value")))
print(hasattr(a, S("missing")))
setattr(a, S("other"), 34)
print(getattr(a, "other"))
