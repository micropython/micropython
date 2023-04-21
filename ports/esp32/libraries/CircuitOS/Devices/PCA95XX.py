import uctypes
from machine import I2C, Pin
import ustruct


class PCA95XX:

	REG_IN = 0x00
	REG_OUT = 0x02
	REG_INV = 0x04
	REG_CONF = 0x06

	def __init__(self, bus: I2C, addr: int):
		self.bus = bus
		self.addr = addr

		self.state_config = 0 | uctypes.UINT16
		self.state_out = 0 | uctypes.UINT16

	def begin(self):
		try:
			self.bus.readfrom_mem(self.addr, 0x00, 1)
		except OSError:
			print("OS Error")
			return False

		self.state_out = 0xffff
		self.reg_write(self.REG_OUT, self.state_out)

		self.reg_write(self.REG_INV, 0 | uctypes.UINT16)

		self.state_config = 0xffff
		self.reg_write(self.REG_CONF, self.state_config)

		return True

	def pin_mode(self, pin: int, mode: int):
		if pin >= 16:
			return

		mask = (1 << pin) | uctypes.UINT16

		if mode == Pin.OUT:
			self.state_config &= ~mask
		elif mode == Pin.IN:
			self.state_config |= mask
		else:
			return

		self.reg_write(self.REG_CONF, self.state_config)

	def pin_write(self, pin: int, state: bool):
		if pin >= 16:
			return

		mask = (1 << pin) | uctypes.UINT16

		if state:
			self.state_out |= mask
		else:
			self.state_out &= ~mask

		self.state_write(self.state_out)

	def state_read(self):
		return self.reg_read(PCA95XX.REG_IN)

	def state_write(self, state: uctypes.UINT16):
		self.state_out = state
		self.reg_write(self.REG_OUT, self.state_out)

	def reg_write(self, reg: int, data: uctypes.UINT16):
		buf = bytearray(2)
		buf[1] = (data >> 8) & 0xff
		buf[0] = data & 0xff
		self.bus.writeto_mem(self.addr, reg, buf)

	def reg_read(self, reg: int):
		data = self.bus.readfrom_mem(self.addr, reg, 2)
		return ustruct.unpack('<H', data)[0]
