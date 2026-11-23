class MyInt(int):
    pass


print(float(MyInt(0)))
print(float(MyInt(2)))
print(float(MyInt(-3)))


class MyIntFloat(int):
    def __float__(self):
        return 42.5


print(float(MyIntFloat(3)))
