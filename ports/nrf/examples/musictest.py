#
# Example usage where "P3" is the Buzzer pin.
#
#  from musictest import play
#  play("P3")
#

from machine import Pin
import music


def play(pin_str):
    p = Pin(pin_str, mode=Pin.OUT)
    music.play(music.PRELUDE, pin=p)
