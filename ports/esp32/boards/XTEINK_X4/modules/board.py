# Xteink X4 button helper
#
# The power button is a direct active-low GPIO. The six navigation buttons
# share a resistor ladder read across two ADC pins: BUTTON_ADC_1 covers
# Back/Confirm/Left/Right, BUTTON_ADC_2 covers Up/Down. ADC thresholds below
# are the calibrated values from the OpenX4 community SDK
# (github.com/open-x4-epaper/community-sdk), verified against this board.

from machine import ADC, Pin
from micropython import const

_pins = Pin.board

BACK = const(0)
CONFIRM = const(1)
LEFT = const(2)
RIGHT = const(3)
UP = const(4)
DOWN = const(5)
POWER = const(6)

# (upper, lower, button) bounds per button: pressed when lower < raw <= upper.
_RANGES_1 = ((3900, 3100, BACK), (3100, 2090, CONFIRM), (2090, 750, LEFT), (750, -1, RIGHT))
_RANGES_2 = ((3900, 1120, UP), (1120, -1, DOWN))

_adc1 = ADC(_pins.BUTTON_ADC_1)
_adc1.atten(ADC.ATTN_11DB)
_adc2 = ADC(_pins.BUTTON_ADC_2)
_adc2.atten(ADC.ATTN_11DB)
_power = Pin(_pins.BUTTON, Pin.IN, Pin.PULL_UP)


def _button_from_adc(raw, ranges):
    for upper, lower, button in ranges:
        if lower < raw <= upper:
            return button
    return -1


def get_state():
    """Return a bitmask of all buttons currently pressed.

    Bit N is set for button N, using the BACK/CONFIRM/LEFT/RIGHT/UP/DOWN/POWER
    constants above.
    """
    state = 0
    button1 = _button_from_adc(_adc1.read(), _RANGES_1)
    if button1 >= 0:
        state |= 1 << button1
    button2 = _button_from_adc(_adc2.read(), _RANGES_2)
    if button2 >= 0:
        state |= 1 << button2
    if not _power.value():
        state |= 1 << POWER
    return state


def is_pressed(button):
    """Return True if the given button constant is currently pressed."""
    return bool(get_state() & (1 << button))
