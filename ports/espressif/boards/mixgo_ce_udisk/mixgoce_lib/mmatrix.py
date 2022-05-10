"""
Matrix

CircuitPython library for Matrix - MixGoCE
=======================================================

Small Cabbage
20210721
"""
import time
from mixgoce import i2c
from matrix import MatrixBackpack16x8 
from adafruit_framebuf import FrameBuffer,MVLSB

buf = bytearray(16)
fb = FrameBuffer(buf, 16, 8, MVLSB)

class MixGoMatrix(MatrixBackpack16x8):

	def show_dynamic(self, to_show, delay=200):
		# self.fill(0)
		if type(to_show)==str or type(to_show)==int:
			for i in to_show:
				fb.fill(0)
				fb.text(i, 5, 0, color=1)
				# turn all LEDs off
				self.fill(0)
				for x in range(16):
					# using the FrameBuffer text result
					bite = buf[x]
					for y in range(8):
						bit = 1 << y & bite
						# if bit > 0 then set the pixel brightness
						if bit:
							self[x, y] = 1
				self.show()
				if len(to_show)>1:
					time.sleep(delay/1000)
		elif type(to_show)==list or type(to_show)==tuple:
			for i in to_show:
				#if type(i)!=str and type(i)!=type(Image.HEART):
				if type(i)!=str and type(i)!=type(bytearray(16)):
					pass
			for i in to_show:
				self.show_dynamic(i)
				time.sleep(delay/1000)
		elif type(to_show)==type(bytearray(16)):
			buf = to_show
			# turn all LEDs off
			self.fill(0)
			# print(buf)
			for x in range(16):
				# using the FrameBuffer text result
				bite = buf[x]
				# print(bite,end=" ")
				for y in range(8):
					bit = 1 << y & bite
					# if bit > 0 then set the pixel brightness
					if bit:
						self[x, y] = 1
			self.show()  
		
	def scroll(self, text_to_show, delay=0):
		for i in range(len(text_to_show) * 6 + 26):
			fb.fill(0)
			fb.text(text_to_show, -i + 16, 0, color=1)
			# turn all LEDs off
			self.fill(0)
			for x in range(16):
				# using the FrameBuffer text result
				bite = buf[x]
				for y in range(8):
					bit = 1 << y & bite
					# if bit > 0 then set the pixel brightness
					if bit:
						self[x, y] = 1
			self.show()
			time.sleep(delay/1000)   
			
	def show_static(self, text_to_show):
		fb.fill(0)
		if len(text_to_show)==2:
			l = 3
		elif len(text_to_show)==1:
			l = 5
		else:
			l = 0
		fb.text(text_to_show, l, 0, color=1)
		# turn all LEDs off
		self.fill(0)
		for x in range(16):
			# using the FrameBuffer text result
			bite = buf[x]
			for y in range(8):
				bit = 1 << y & bite
				# if bit > 0 then set the pixel brightness
				if bit:
					self[x, y] = 1
		self.show()
	
	def set_brightness(self, val):
		self.brightness = max(min(val, 1), 0)
		
	def get_brightness(self):
		return self.brightness
	
	def set_pixel(self, x, y, val):
		self[x, y] = val
		self.show()
	
	def get_pixel(self, x, y):
		return int(self[x, y])
	
	def clear(self):
		self.fill(0)
		self.show()
	
	def up(self, times, rotate=False):
		for i in range(times):
			self.shift_down(rotate)
		self.show()
	
	def down(self, times, rotate=False):
		for i in range(times):
			self.shift_up(rotate)
		self.show()
		
	def left(self, times, rotate=False):
		for i in range(times):
			self.shift_left(rotate)
		self.show()
	
	def right(self, times, rotate=False):
		for i in range(times):
			self.shift_right(rotate)
		self.show()
		
display = MixGoMatrix(i2c, auto_write=False, brightness=0.1)
display.clear()