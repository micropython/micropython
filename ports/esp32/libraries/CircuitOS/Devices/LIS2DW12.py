from machine import I2C
from micropython import const


class LIS2DW12:
	REG_CTRL1 = const(0x20)
	REG_CTRL2 = const(0x21)
	REG_CTRL3 = const(0x22)
	REG_CTRL6 = const(0x25)
	REG_OUT_X_L = const(0x28)
	REG_OUT_Y_L = const(0x2A)
	REG_OUT_Z_L = const(0x2C)

	def __init__(self, i2c: I2C, addr=0x18):
		self.i2c = i2c
		self.addr = addr

	def begin(self):
		try:
			# ODR=5 MODE=0 LP=1
			self.setreg(self.REG_CTRL1, 0x51)
		except OSError:
			return False

		# BDU=1
		self.setreg(self.REG_CTRL2, 0x0C)
		# SLP_MODE_SEL=1
		self.setreg(self.REG_CTRL3, 0x02)
		# scale=2G
		self.r_w_reg(self.REG_CTRL6, 0, 0xCF)
		# one-shot: Off
		self.r_w_reg(self.REG_CTRL1, 0, 0xF3)

		return True

	def int16(self, d):
		return d if d < 0x8000 else d - 0x10000

	def setreg(self, reg: int, dat: int):
		self.i2c.writeto_mem(self.addr, reg, bytes([dat]))

	def getreg(self, reg: int):
		return self.i2c.readfrom_mem(self.addr, reg, 1)[0]

	def get2reg(self, reg: int):
		return self.getreg(reg) + self.getreg(reg + 1) * 256

	def r_w_reg(self, reg: int, data: int, mask: int):
		val = self.getreg(reg)
		val = (val & mask) | data
		self.setreg(reg, val)

	def mg(self, reg):
		return self.int16(self.get2reg(reg)) * 0.061 / 1000

	def get_x(self) -> float:
		return self.mg(self.REG_OUT_X_L)

	def get_y(self) -> float:
		return self.mg(self.REG_OUT_Y_L)

	def get_z(self) -> float:
		return self.mg(self.REG_OUT_Z_L)

	def get_accel(self) -> tuple:
		x = self.get_x()
		y = self.get_y()
		z = self.get_z()
		return x, y, z
