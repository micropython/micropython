class C(Exception):
    def __init__(self):
        raise self


class C1(C):
    pass


class B:
    pass


class C2(B, Exception):
    def __init__(self):
        raise self


class C3(Exception, B):
    def __init__(self):
        raise self


class D(Exception):
    pass


class C4(D):
    def __init__(self):
        raise self


for cls in C, C1, C2, C3, C4:
    try:
        cls()
    except TypeError as e:
        print("TypeError")
