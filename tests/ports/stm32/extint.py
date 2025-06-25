import pyb

# test basic functionality
pin = pyb.Pin.cpu.A4
ext = pyb.ExtInt(pin, pyb.ExtInt.IRQ_RISING, pyb.Pin.PULL_DOWN, lambda l: print("line:", l))
ext.disable()
ext.enable()
print(ext.line())
ext.swint()

# test swint while disabled, then again after re-enabled
ext.disable()
ext.swint()
ext.enable()
ext.swint()

# disable now that the test is finished
ext.disable()
