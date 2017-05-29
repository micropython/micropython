from machine import Signal

# 96Boards Carbon board
# USR1 - User controlled led, connected to PD2
# USR2 - User controlled led, connected to PA15
# BT - Bluetooth indicator, connected to PB5.
# Note - 96b_carbon uses (at the time of writing) non-standard
# for Zephyr port device naming convention.
LED = Signal(("GPIOA", 15), Pin.OUT)
