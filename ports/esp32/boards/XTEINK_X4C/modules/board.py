import time
from binascii import unhexlify

from machine import I2C, Pin
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

_charge_stat = Pin(_pins.CHARGE_STAT, Pin.IN)

_CW2017_ADDRESS = const(0x63)
_CW2017_REG_VERSION = const(0x00)
_CW2017_REG_VCELL = const(0x02)
_CW2017_REG_SOC = const(0x04)
_CW2017_REG_MODE = const(0x08)
_CW2017_REG_SOC_ALERT = const(0x0B)
_CW2017_REG_BATINFO = const(0x10)
_CW2017_UPDATE_FLAG = const(0x80)
_CW2017_MODE_NORMAL = const(0x00)
_CW2017_MODE_RESTART = const(0x30)
_CW2017_MODE_DEFAULT = const(0xF0)

_CW2017_BATINFO = unhexlify(
    "5000000000000000b7c3c6c2baacc6a593ffe0ab8d67574b41382c714ed98dd1"
    "cecccbc9c7c4c1bbb9bdc29d8e847c736b71828fa293588c0000ab0200000000"
    "000064000000000000000000000000c2"
)

_i2c = None


def _gauge_i2c() -> I2C:
    global _i2c
    if _i2c is None:
        _i2c = I2C(0, freq=400000)
    return _i2c


def _gauge_read(register: int, length: int = 1) -> bytes:
    return _gauge_i2c().readfrom_mem(_CW2017_ADDRESS, register, length)


def _gauge_write(register: int, value: int) -> None:
    _gauge_i2c().writeto_mem(_CW2017_ADDRESS, register, bytes((value,)))


def _gauge_ready() -> bool:
    version = _gauge_read(_CW2017_REG_VERSION)[0]
    alert = _gauge_read(_CW2017_REG_SOC_ALERT)[0]
    return (version & 0xFD) == 0x0D and bool(alert & _CW2017_UPDATE_FLAG)


def _gauge_load_profile() -> bool:
    if _gauge_read(_CW2017_REG_SOC_ALERT)[0] & _CW2017_UPDATE_FLAG:
        return False
    for offset, value in enumerate(_CW2017_BATINFO):
        _gauge_write(_CW2017_REG_BATINFO + offset, value)
    _gauge_write(_CW2017_REG_SOC_ALERT, _CW2017_UPDATE_FLAG)
    _gauge_write(_CW2017_REG_MODE, _CW2017_MODE_DEFAULT)
    time.sleep_ms(30)
    _gauge_write(_CW2017_REG_MODE, _CW2017_MODE_RESTART)
    time.sleep_ms(30)
    _gauge_write(_CW2017_REG_MODE, _CW2017_MODE_NORMAL)
    for _ in range(30):
        time.sleep_ms(100)
        if _gauge_ready():
            return True
    return False


def get_battery_voltage() -> float:
    raw = _gauge_read(_CW2017_REG_VCELL, 2)
    vcell = ((raw[0] << 8) | raw[1]) & 0x3FFF
    return vcell * 5 / 16 / 1000


def get_battery_percentage() -> int:
    if not _gauge_ready() and not _gauge_load_profile():
        raise OSError("battery gauge not ready")
    return min(100, _gauge_read(_CW2017_REG_SOC)[0])


def is_charging() -> bool:
    return bool(_charge_stat.value())


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
