try:
    import umachine as machine
except ImportError:
    import machine
try:
    machine.PinBase
except AttributeError:
    print("SKIP")
    import sys
    sys.exit()


class MyPin(machine.PinBase):

    def __init__(self):
        print("__init__")
        self.v = False

    def value(self, v=None):
        print("value:", v)
        if v is None:
            self.v = not self.v
            return int(self.v)

p = MyPin()

print(p.value())
print(p.value())
print(p.value())
p.value(1)
p.value(0)
