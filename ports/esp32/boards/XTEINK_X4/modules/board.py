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

BACK: int = const(0)
CONFIRM: int = const(1)
LEFT: int = const(2)
RIGHT: int = const(3)
UP: int = const(4)
DOWN: int = const(5)
POWER: int = const(6)

# (upper, lower, button) bounds per button: pressed when lower < raw <= upper.
_RANGES_1 = ((3900, 3100, BACK), (3100, 2090, CONFIRM), (2090, 750, LEFT), (750, -1, RIGHT))
_RANGES_2 = ((3900, 1120, UP), (1120, -1, DOWN))

_adc1 = ADC(_pins.BUTTON_ADC_1)
_adc1.atten(ADC.ATTN_11DB)
_adc2 = ADC(_pins.BUTTON_ADC_2)
_adc2.atten(ADC.ATTN_11DB)
_power = Pin(_pins.BUTTON, Pin.IN, Pin.PULL_UP)

# Battery voltage is halved by a 2:1 divider before reaching the ADC pin.
_BATTERY_DIVIDER_MULTIPLIER = 2.0

_adc_bat = ADC(_pins.BAT_ADC)
_adc_bat.atten(ADC.ATTN_11DB)


def _button_from_adc(raw: int, ranges: tuple) -> int:
    for upper, lower, button in ranges:
        if lower < raw <= upper:
            return button
    return -1


def _get_state() -> int:
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


def get_battery_voltage() -> float:
    """Return the battery voltage in volts, accounting for the divider."""
    millivolts = _adc_bat.read_uv() / 1000 * _BATTERY_DIVIDER_MULTIPLIER
    return millivolts / 1000


def get_battery_percentage() -> int:
    """Return an estimated battery charge percentage (0-100).

    Uses a cubic fit derived from real LiPo discharge samples, ported from
    the OpenX4 community SDK's BatteryMonitor.
    """
    v = get_battery_voltage()
    percent = -144.9390 * v**3 + 1655.8629 * v**2 - 6158.8520 * v + 7501.3202
    return round(min(100.0, max(0.0, percent)))


class buttons:
    """machine.Pin-like objects for each button, e.g. board.buttons.Back()."""

    class _Button:
        _bit = None

        def value(self) -> int:
            return 1 if _get_state() & (1 << self._bit) else 0

        def __call__(self) -> int:
            return self.value()

    class Back(_Button):
        _bit: int = BACK

    class Confirm(_Button):
        _bit: int = CONFIRM

    class Left(_Button):
        _bit: int = LEFT

    class Right(_Button):
        _bit: int = RIGHT

    class Up(_Button):
        _bit: int = UP

    class Down(_Button):
        _bit: int = DOWN

    class Power(_Button):
        _bit: int = POWER
