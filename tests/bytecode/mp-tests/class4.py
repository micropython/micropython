class A:
    def __init__(self, x):
        self.x = x
        self.y = 0

    def get(self):
        return self.x + self.y
A(1)
A(2).get()
