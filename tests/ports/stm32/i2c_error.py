# test I2C errors, with polling (disabled irqs) and DMA

import pyb
from pyb import I2C

if not hasattr(pyb, "Accel"):
    print("SKIP")
    raise SystemExit

# init accelerometer
pyb.Accel()

# get I2C bus
i2c = I2C(1, I2C.CONTROLLER, dma=True)

# test polling mem_read
pyb.disable_irq()
i2c.mem_read(1, 76, 0x0A)  # should succeed
pyb.enable_irq()
try:
    pyb.disable_irq()
    i2c.mem_read(1, 77, 0x0A)  # should fail
except OSError as e:
    pyb.enable_irq()
    print(repr(e))
i2c.mem_read(1, 76, 0x0A)  # should succeed

# test polling mem_write
pyb.disable_irq()
i2c.mem_write(1, 76, 0x0A)  # should succeed
pyb.enable_irq()
try:
    pyb.disable_irq()
    i2c.mem_write(1, 77, 0x0A)  # should fail
except OSError as e:
    pyb.enable_irq()
    print(repr(e))
i2c.mem_write(1, 76, 0x0A)  # should succeed

# test DMA mem_read
i2c.mem_read(1, 76, 0x0A)  # should succeed
try:
    i2c.mem_read(1, 77, 0x0A)  # should fail
except OSError as e:
    print(repr(e))
i2c.mem_read(1, 76, 0x0A)  # should succeed

# test DMA mem_write
i2c.mem_write(1, 76, 0x0A)  # should succeed
try:
    i2c.mem_write(1, 77, 0x0A)  # should fail
except OSError as e:
    print(repr(e))
i2c.mem_write(1, 76, 0x0A)  # should succeed
