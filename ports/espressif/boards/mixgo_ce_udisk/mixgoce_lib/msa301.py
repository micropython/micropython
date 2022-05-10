"""
MSA301

CircuitPython library for the MSA301 Accelerometer
=======================================================

dahanzimin
20210411
mixly 
"""
import time
import adafruit_bus_device.i2c_device as i2c_device
from micropython import const

MSA301_ADDRESS	     		= const(0x26)

MSA301_REG_DEVICE_ID	 	= const(0x01)
MSA301_REG_DATA	     		= const(0x02)
MSA301_REG_ODR 				= const(0x10)
MSA301_REG_POWERMODE 		= const(0x11)
MSA301_REG_RESRANGE 		= const(0x0F)


_STANDARD_GRAVITY = 9.806

class MSA301:

	_BUFFER = bytearray(2)
	data_reg = bytearray(6)
	
	def __init__(self, i2c_bus):
		self._device = i2c_device.I2CDevice(i2c_bus, MSA301_ADDRESS)
		if self._chip_id() != 0x13:
			raise AttributeError("Cannot find a MSA301")
			
		self._write_u8(MSA301_REG_ODR,0X09)  		#RATE_500_HZ
		self._write_u8(MSA301_REG_POWERMODE,0X12)  	#NORMAL & WIDTH_250_HZ
		self._write_u8(MSA301_REG_RESRANGE,0X02)  	#RESOLUTION_14_BIT & RANGE_8_G
	
	def _read_u8(self, address):
        # Read an 8-bit unsigned value from the specified 8-bit address.
		with self._device:
			self._BUFFER[0] = address & 0xFF
			self._device.write(self._BUFFER, end=1)
			self._device.readinto(self._BUFFER, end=1)
		return self._BUFFER[0]
		
	def _write_u8(self, address, val):
		# Write an 8-bit unsigned value to the specified 8-bit address.
		with self._device:
			self._BUFFER[0] = address & 0xFF
			self._BUFFER[1] = val & 0xFF
			self._device.write(self._BUFFER, end=2)

	def _chip_id(self):  
		return self._read_u8(MSA301_REG_DEVICE_ID)
		
	def u2s(self,n):
		return n if n < (1 << 7) else n - (1 << 8)	
	
	@property
	def acceleration(self): 
		for i in range(0,6):
			self.data_reg[i]=self._read_u8(MSA301_REG_DATA+i)
		x_acc=((self.u2s(self.data_reg[1])<<8|self.data_reg[0])>>2)/1024.0
		y_acc=((self.u2s(self.data_reg[3])<<8|self.data_reg[2])>>2)/1024.0
		z_acc=((self.u2s(self.data_reg[5])<<8|self.data_reg[4])>>2)/1024.0
		return (-y_acc,-x_acc,z_acc)


