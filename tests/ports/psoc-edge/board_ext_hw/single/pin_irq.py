from machine import Pin

# Two pins are connected together.
# An output pin triggers the interrupt.
# An input pin is configured to receive the interrupt
pin_trigger = Pin("P16_0", mode=Pin.OUT, pull=Pin.PULL_UP, value=True)
pin_irq = Pin("P16_1", mode=Pin.IN)


def irq_handler():
    print("irq")


# Configure the IRQ handler to trigger on falling edge
# The handler should be called twice
pin_irq.irq(irq_handler, trigger=Pin.IRQ_FALLING)
pin_trigger(0)
pin_trigger(1)
pin_trigger(0)

# Now it is configured to trigger on rising edge.
# The handler should be called twice
pin_irq.irq(irq_handler, trigger=Pin.IRQ_RISING)
pin_trigger(0)
pin_trigger(1)
pin_trigger(0)
pin_trigger(1)

# TODO: The priority and ishard parameters are not tested.
# These might be added in the future if we find a suitable HIL functional test.

# Wait for the interrupts to be processed
import time

time.sleep_ms(500)
