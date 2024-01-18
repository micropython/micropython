import pyb


def test_irq():
    # test basic disable/enable
    i1 = pyb.disable_irq()
    print(i1)
    pyb.enable_irq()  # by default should enable IRQ

    # check that interrupts are enabled by waiting for ticks
    pyb.delay(10)

    # check nested disable/enable
    i1 = pyb.disable_irq()
    i2 = pyb.disable_irq()
    print(i1, i2)
    pyb.enable_irq(i2)
    pyb.enable_irq(i1)

    # check that interrupts are enabled by waiting for ticks
    pyb.delay(10)


test_irq()
