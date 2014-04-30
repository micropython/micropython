import pyb

ext = pyb.ExtInt('X1', pyb.ExtInt.IRQ_RISING, pyb.Pin.PULL_DOWN, lambda l:print('line:', l))
ext.disable()
ext.enable()
print(ext.line())
ext.swint()
ext.disable()
