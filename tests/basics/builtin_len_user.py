# test builtin len with user-defined __len__

class A:
    def __len__(self):
        return 3


print(len(A()))


class B:
    def __len__(self):
        return None


try:
    len(B())
except TypeError:
    print("TypeError")


class C:
    def __len__(self):
        return ""


try:
    len(C())
except TypeError:
    print("TypeError")


class D:
    def __len__(self):
        return ()


try:
    len(D())
except TypeError:
    print("TypeError")
