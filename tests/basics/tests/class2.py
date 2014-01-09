# class with __init__

class C1:
    def __init__(self):
        self.x = 1

c1 = C1()
print(type(c1) == C1)
print(c1.x)

class C2:
    def __init__(self, x):
        self.x = x

c2 = C2(4)
print(type(c2) == C2)
print(c2.x)
