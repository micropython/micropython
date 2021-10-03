try:
    try:
        import umachine as machine
    except ImportError:
        import machine
    machine.PinBase
except:
    print("SKIP")
    raise SystemExit


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
