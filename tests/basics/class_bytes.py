class C1:
    def __init__(self, value):
        self.value = value

    def __bytes__(self):
        return self.value

c1 = C1(b"class 1")
print(bytes(c1))
