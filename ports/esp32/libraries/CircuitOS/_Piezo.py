import time
from machine import Pin, PWM


class Piezo:

	def __init__(self, pin: int):
		self.__pin = Pin(pin, mode=Pin.OUT)

	def tone(self, freq: int, duration: int):
		pwm = PWM(self.__pin, freq=freq, duty_u16=round(65535/2))
		time.sleep_ms(duration)
		pwm.deinit()
