# test machine.Signal class

try:
    try:
        import umachine as machine
    except ImportError:
        import machine
    machine.PinBase
    machine.Signal
except:
    print("SKIP")
    raise SystemExit

class Pin(machine.PinBase):
    def __init__(self):
        self.v = 0

    def value(self, v=None):
        if v is None:
            return self.v
        else:
            self.v = int(v)


# test non-inverted
p = Pin()
s = machine.Signal(p)
s.value(0)
print(p.value(), s.value())
s.value(1)
print(p.value(), s.value())

# test inverted, and using on/off methods
p = Pin()
s = machine.Signal(p, invert=True)
s.off()
print(p.value(), s.value())
s.on()
print(p.value(), s.value())
