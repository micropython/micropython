# Test basic use of I2CTarget and a memory buffer.
#
# Requires two instances with their SCL and SDA lines connected together.
# Any combination of the below supported boards can be used.
#
# Notes:
# - pull-up resistors may be needed
# - alif use 1.8V signalling

import sys
from machine import I2C, I2CTarget

ADDR = 67

# Configure pins based on the target.
if sys.platform == "alif":
    i2c_args = (1,)  # pins P3_7/P3_6
    i2c_kwargs = {}
elif sys.platform == "esp32":
    i2c_args = (1,)  # on pins 9/8
    i2c_kwargs = {}
elif sys.platform == "mimxrt":
    i2c_args = (0,)  # pins 19/18 on Teensy 4.x
    i2c_kwargs = {}
elif sys.platform == "rp2":
    i2c_args = (0,)
    i2c_kwargs = {"scl": 9, "sda": 8}
elif sys.platform == "pyboard":
    i2c_args = ("Y",)
    i2c_kwargs = {}
elif sys.platform == "samd":
    i2c_args = ()  # pins SCL/SDA
    i2c_kwargs = {}
elif "zephyr-rpi_pico" in sys.implementation._machine:
    i2c_args = ("i2c1",)  # on gpio7/gpio6
    i2c_kwargs = {}
else:
    print("Please add support for this test on this platform.")
    raise SystemExit


def simple_irq(i2c_target):
    flags = i2c_target.irq().flags()
    if flags & I2CTarget.IRQ_END_READ:
        print("IRQ_END_READ", i2c_target.memaddr)
    if flags & I2CTarget.IRQ_END_WRITE:
        print("IRQ_END_WRITE", i2c_target.memaddr)


# I2C controller
def instance0():
    i2c = I2C(*i2c_args, **i2c_kwargs)
    multitest.next()
    for iteration in range(2):
        print("controller iteration", iteration)
        multitest.wait("target stage 1")
        i2c.writeto_mem(ADDR, 2 + iteration, "0123")
        multitest.broadcast("controller stage 2")
        multitest.wait("target stage 3")
        print(i2c.readfrom_mem(ADDR, 2 + iteration, 4))
        multitest.broadcast("controller stage 4")
    print("done")


# I2C target
def instance1():
    buf = bytearray(b"--------")
    i2c_target = I2CTarget(*i2c_args, **i2c_kwargs, addr=ADDR, mem=buf)
    i2c_target.irq(simple_irq)
    multitest.next()
    for iteration in range(2):
        print("target iteration", iteration)
        multitest.broadcast("target stage 1")
        multitest.wait("controller stage 2")
        print(buf)
        multitest.broadcast("target stage 3")
        multitest.wait("controller stage 4")
    i2c_target.deinit()
    print("done")
