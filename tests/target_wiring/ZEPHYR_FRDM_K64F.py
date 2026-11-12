# Target wiring for zephyr frdm_k64f.
#
# Connect:
# - TX=PTC17 to RX=PTC16

pin_loopback_pins = [(("gpioc", 16), ("gpioc", 17))]
adc_loopback_pins = []  # TODO work out how to refer to ADC pins

uart_loopback_args = ("uart3",)
uart_loopback_kwargs = {}  # TX/RX=PTC17/PTC16
