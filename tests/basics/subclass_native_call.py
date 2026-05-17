# test calling a subclass of a native class that supports calling

# For this test we need a native class that can be subclassed (has make_new)
# and is callable (has call).  The only one available is machine.Signal, which
# in turns needs PinBase.
try:
    import machine
    machine.PinBase
    machine.Signal
except:
    print("SKIP")
    raise SystemExit

class Pin(machine.PinBase):
    #def __init__(self):
    #    self.v = 0

    def value(self, v=None):
        return 42

class MySignal(machine.Signal):
    pass

s = MySignal(Pin())

# apply call to the subclass, which should call the native base
print(s())
