from machine import Pin, SPI
from time import sleep_us, sleep_ms
from micropython import const
from .Data import DriveInfo
from .Pins import *
from ustruct import unpack


class S3Error:
	OK = const(0)
	Camera = const(1)
	Update = const(2)


class S3Interface:
	
	class __Command:
		ID = const(0x1)
		Mode = const(0x2)
		Frame = const(0x3)
		Proximity = const(0x4)
		FrameGray = const(0x5)
		Hue = const(0x6)
		Shake = const(0x7)
		Shutdown = const(0x8)
		Error = const(0x9)
		Version = const(0x10)

	S3_ID = const(0x15)
	MAX_FRAME_SIZE = const(10000)

	def __init__(self):
		self.recv_buf = bytearray(self.MAX_FRAME_SIZE)
		self.spi = SPI(2, sck=Pin(Pins.S3_CLK), miso=Pin(Pins.S3_MISO), mosi=Pin(Pins.S3_MOSI), baudrate=20_000_000, firstbit=SPI.MSB)
		self.spi.deinit()

		#####

		self.__miso = Pin(Pins.S3_MISO, mode=Pin.OUT, value=1)
		self.__clk = Pin(Pins.S3_CLK, mode=Pin.OUT, value=1)
		self.__cs = Pin(Pins.S3_CS, mode=Pin.OUT, value=1)
		self.__rst = Pin(Pins.S3_RST, mode=Pin.OUT, value=1)

		self.__int = Pin(Pins.S3_INT, mode=Pin.IN, drive=Pin.PULL_DOWN)

		# S3 GPIO2 is input-low during boot. The line on the WROOM side should be high-impedance while the S3 is booting
		self.__mosi = Pin(Pins.S3_MOSI, mode=Pin.IN)

	def begin(self):
		self.reset()

		self.send(self.__Command.ID)
		self.wait_ready()
		id_ = self.recv()

		return id_ == self.S3_ID

	def end(self):
		self.send(self.__Command.Shutdown)
		sleep_ms(100)
		self.spi.deinit()
		self.__cs.init(mode=Pin.IN)
		self.__rst.value(0)
		self.__rst.init(mode=Pin.IN)

	def init_pins(self):
		for pin in [self.__miso, self.__clk, self.__cs, self.__rst]:
			pin.init(mode=Pin.OUT, value=1)

		self.__int.init(mode=Pin.IN, pull=Pin.PULL_DOWN)

		# S3 GPIO2 is input-low during boot. The line on the WROOM side should be high-impedance while the S3 is booting
		self.__mosi.init(mode=Pin.IN)

	def reset(self):
		self.spi.deinit()
		self.init_pins()

		self.__rst.value(0)
		sleep_ms(5)

		self.__rst.value(1)
		sleep_ms(5)

		self.wait_ready()

		self.__mosi.init(mode=Pin.OUT)
		self.spi.init()

	def get_error(self):
		self.send(self.__Command.Error)
		self.wait_ready()
		return self.recv()

	def get_version(self):
		self.send(self.__Command.Version)
		self.wait_ready()
		return self.recv()

	def set_mode(self, mode):
		self.send(self.__Command.Mode, mode)

	def get_frame_gray(self):
		self.send(self.__Command.FrameGray)
		self.wait_ready()

		self.__cs.value(0)
		self.spi.readinto(self.recv_buf, 160 * 120)
		self.__cs.value(1)

		return self.recv_buf[:160 * 120]

	def get_frame(self):
		self.send(self.__Command.Frame)
		self.wait_ready()

		self.__cs.value(0)

		size = unpack('I', self.spi.read(4))[0]

		dataSize = size
		if dataSize % 4 != 0:
			dataSize += 4 - (dataSize % 4)

		if dataSize > self.MAX_FRAME_SIZE:
			self.clear_data(dataSize)
			return

		if size == 0:
			return

		self.spi.readinto(self.recv_buf, dataSize)
		self.__cs.value(1)

		return DriveInfo.deserialize(self.recv_buf)


	def set_hue(self, hue: int):
		self.send(self.__Command.Hue, hue)

	def set_shake(self, shake: bool):
		self.send(self.__Command.Shake, shake)

	def send(self, byte1, byte2=0, byte3=0, byte4=0):
		data = bytearray([byte1, byte2, byte3, byte4])
		self.send_data(data)

	def send_data(self, data: list|tuple|bytearray):
		self.wait_ready()
		self.__cs.value(0)
		self.spi.write(data)
		self.__cs.value(1)

	def recv(self):
		data = bytearray(4)
		self.recv_data(data, 4)
		return data[0]

	def recv_data(self, data: bytearray, count: int):
		self.wait_ready()
		self.__cs.value(0)
		self.spi.readinto(data, count)
		self.__cs.value(1)

	def clear_data(self, count: int):
		total = 0
		while total < count:
			current = min(self.MAX_FRAME_SIZE, count - total)
			self.spi.readinto(self.recv_buf, current)
			total += current

	def wait_ready(self):
		sleep_us(50)
		while self.__int.value() == 0:
			sleep_us(50)
