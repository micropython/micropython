# Target wiring for zephyr frdm_k64f.
#
# Connect:
# - TX=PTC17 to RX=PTC16

uart_loopback_args = ("uart3",)
uart_loopback_kwargs = {}  # TX/RX=PTC17/PTC16
