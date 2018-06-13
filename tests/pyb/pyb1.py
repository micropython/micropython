# basic tests of pyb module

import pyb

# test delay

pyb.delay(-1)
pyb.delay(0)
pyb.delay(1)

start = pyb.millis()
pyb.delay(17)
print((pyb.millis() - start) // 5) # should print 3

# test udelay

pyb.udelay(-1)
pyb.udelay(0)
pyb.udelay(1)

start = pyb.millis()
pyb.udelay(17000)
print((pyb.millis() - start) // 5) # should print 3

# other

pyb.disable_irq()
pyb.enable_irq()

print(pyb.have_cdc())

pyb.hid((0, 0, 0, 0)) # won't do anything

pyb.sync()

print(len(pyb.unique_id()))

pyb.wfi()

pyb.fault_debug(True)
pyb.fault_debug(False)
