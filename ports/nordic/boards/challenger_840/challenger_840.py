import board
import digitalio

_LDO_PIN = digitalio.DigitalInOut(board.LDO_CONTROL)
_LDO_PIN.direction = digitalio.Direction.OUTPUT
_LDO_PIN.value = True


def ldo_on():
    global _LDO_PIN
    _LDO_PIN.value = True


def ldo_off():
    global _LDO_PIN
    _LDO_PIN.value = False
