# In general inheriting from native types is a bit broken on MicroPython.
# As a result, classes that decend from native types do NOT inherit their
# implicit casting to float or complex and have to do it for themselves.


class T1(int):
    pass


class T2(int):
    def __float__(self):
        return float(int(self))


for test in [T1("123"), T2("123")]:
    try:
        print(float(test))
    except TypeError:
        print("TypeError")
