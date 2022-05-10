"""
MXC6655XA

CircuitPython library for the MXC6655XA Accelerometer
=======================================================

dahanzimin
20210411
mixly 
"""
import time
import adafruit_bus_device.i2c_device as i2c_device
from micropython import const

MXC6655XA_ADDRESS	     = const(0x15)

MXC6655XA_REG_DATA	     = const(0x03)
MXC6655XA_REG_CTRL	     = const(0x0D)
MXC6655XA_REG_DEVICE_ID	 = const(0x0E)

MXC6655XA_CMD_8G_POWER_ON  = const(0x40)
MXC6655XA_CMD_4G_POWER_ON  = const(0x20)
MXC6655XA_CMD_2G_POWER_ON  = const(0x00)

MXC6655XA_2G_SENSITIVITY  =1024
MXC6655XA_4G_SENSITIVITY  =512
MXC6655XA_8G_SENSITIVITY  =256
MXC6655XA_T_ZERO          =25
MXC6655XA_T_SENSITIVITY	  =0.586

class MXC6655XA:

	_BUFFER = bytearray(2)
	data_reg = bytearray(7)
	
	def __init__(self, i2c_bus):
		self._device = i2c_device.I2CDevice(i2c_bus, MXC6655XA_ADDRESS)
		if self._chip_id() != 0x02:
			raise AttributeError("Cannot find a MXC6655XA")
		self._Enable()  #star
		time.sleep(0.3)
		self.range = MXC6655XA_8G_SENSITIVITY #SET 8g range
	
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
		return self._read_u8(MXC6655XA_REG_DEVICE_ID)
		
	def _Enable(self): 
		self._write_u8(MXC6655XA_REG_CTRL,MXC6655XA_CMD_8G_POWER_ON)
		
	def u2s(self,n):
		return n if n < (1 << 7) else n - (1 << 8)	
	
	@property
	def acceleration(self): 
		for i in range(0,6):
			self.data_reg[i]=self._read_u8(MXC6655XA_REG_DATA+i)
		x_acc=float((self.u2s(self.data_reg[0])<<8|self.data_reg[1])>>4)/self.range 
		y_acc=float((self.u2s(self.data_reg[2])<<8|self.data_reg[3])>>4)/self.range 
		z_acc=float((self.u2s(self.data_reg[4])<<8|self.data_reg[5])>>4)/self.range 
		#t_acc=float(self.u2s(self.data_reg[6]))*MXC6655XA_T_SENSITIVITY + MXC6655XA_T_ZERO
		return (-y_acc,-x_acc,z_acc)

	def get_temperature(self):
		t_acc=float(self.u2s(self._read_u8(MXC6655XA_REG_DATA+6)))*MXC6655XA_T_SENSITIVITY + MXC6655XA_T_ZERO
		return round(t_acc,1)
