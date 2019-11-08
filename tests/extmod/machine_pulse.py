try:
    try:
        import umachine as machine
    except ImportError:
        import machine
    machine.PinBase
    machine.time_pulse_us
except:
    print("SKIP")
    raise SystemExit


class ConstPin(machine.PinBase):

    def __init__(self, value):
        self.v = value

    def value(self, v=None):
        if v is None:
            return self.v
        else:
            self.v = v


class TogglePin(machine.PinBase):

    def __init__(self):
        self.v = 0

    def value(self, v=None):
        if v is None:
            self.v = 1 - self.v
            print("value:", self.v)
            return self.v


p = TogglePin()

t = machine.time_pulse_us(p, 1)
print(type(t))
t = machine.time_pulse_us(p, 0)
print(type(t))

p = ConstPin(0)
print(machine.time_pulse_us(p, 1, 10))
print(machine.time_pulse_us(p, 0, 10))
