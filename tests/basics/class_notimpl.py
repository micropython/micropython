# Test that returning of NotImplemented from binary op methods leads to
# TypeError.
try:
    NotImplemented
except NameError:
    print("SKIP")
    raise SystemExit

class C:
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return "C(%s)" % self.value

    def __add__(self, rhs):
        print(self, '+', rhs)
        return NotImplemented

    def __sub__(self, rhs):
        print(self, '-', rhs)
        return NotImplemented

    def __lt__(self, rhs):
        print(self, '<', rhs)
        return NotImplemented

    def __neg__(self):
        print('-', self)
        return NotImplemented

c = C(0)

try:
    c + 1
except TypeError:
    print("TypeError")

try:
    c - 2
except TypeError:
    print("TypeError")

try:
    c < 1
except TypeError:
    print("TypeError")

# NotImplemented isn't handled specially in unary methods
print(-c)
