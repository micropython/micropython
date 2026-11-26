# test conversion of int subclass instances to complex
# requires: complex


class MyInt(int):
    pass


print(complex(MyInt(2)))
print(complex(MyInt(2), MyInt(3)))
print(abs(complex(MyInt(3), MyInt(4))))
