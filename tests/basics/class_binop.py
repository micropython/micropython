class foo(object):
    def __init__(self, value):
        self.x = value

    def __eq__(self, other):
        print('eq')
        return self.x == other.x

    def __lt__(self, other):
        print('lt')
        return self.x < other.x

    def __gt__(self, other):
        print('gt')
        return self.x > other.x

    def __le__(self, other):
        print('le')
        return self.x <= other.x

    def __ge__(self, other):
        print('ge')
        return self.x >= other.x

for i in range(3):
    for j in range(3):
        print(foo(i) == foo(j))
        print(foo(i) < foo(j))
        print(foo(i) > foo(j))
        print(foo(i) <= foo(j))
        print(foo(i) >= foo(j))
