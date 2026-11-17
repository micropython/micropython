# Target wiring for zephyr rpi_pico.

from machine import Pin

i2c_loopback_controller_kwargs = {"scl": Pin(("gpio0", 5)), "sda": Pin(("gpio0", 4))}
i2c_loopback_target_args = ("i2c1",)  # on gpio7/gpio6
i2c_loopback_target_kwargs = {}
