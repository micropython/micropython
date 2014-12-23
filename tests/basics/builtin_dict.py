class A:
    def __init__(self):
        self.a=1
        self.b=2

try:
    d=A().__dict__
    print(d['a'])
    print(d['b'])
except AttributeError:
    print("SKIP")
