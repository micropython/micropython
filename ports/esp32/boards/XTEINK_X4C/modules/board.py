from machine import Pin
from micropython import const

_pins = Pin.board

BACK: int = const(0)
CONFIRM: int = const(1)
LEFT: int = const(2)
RIGHT: int = const(3)
UP: int = const(4)
DOWN: int = const(5)
POWER: int = const(6)

_BUTTON_PINS = (
    _pins.BUTTON_BACK,
    _pins.BUTTON_CONFIRM,
    _pins.BUTTON_LEFT,
    _pins.BUTTON_RIGHT,
    _pins.BUTTON_UP,
    _pins.BUTTON_DOWN,
    _pins.BUTTON_POWER,
)

for _pin in _BUTTON_PINS:
    _pin.init(Pin.IN, Pin.PULL_UP)
del _pin


class buttons:
    class _Button:
        _bit = None

        def value(self) -> int:
            return 0 if _BUTTON_PINS[self._bit].value() else 1

        def __call__(self) -> int:
            return self.value()

        def irq(self, handler=None, trigger=Pin.IRQ_FALLING):
            return _BUTTON_PINS[self._bit].irq(handler, trigger)

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
