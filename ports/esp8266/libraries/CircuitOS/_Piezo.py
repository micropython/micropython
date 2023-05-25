import time
from machine import Pin, PWM


class Piezo:

	def __init__(self, pin: int):
		self.__pin = Pin(pin, mode=Pin.OUT)

	def tone(self, freq: int, duration: int):
		pwm = PWM(self.__pin, freq=freq, duty=round(1024 / 2))
		time.sleep_ms(duration)
		pwm.deinit()
