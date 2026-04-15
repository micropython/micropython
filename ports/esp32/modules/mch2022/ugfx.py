import display, gc, buttons

display_flags = 0

def orientation(deg=None):
	if deg:
		display.orientation(deg)
	else:
		return display.orientation()

activeList = None
listUpCallback = None
listDownCallback = None

BTN_A      = buttons.BTN_A
BTN_B      = buttons.BTN_B
BTN_START  = buttons.BTN_START
BTN_SELECT = buttons.BTN_SELECT
JOY_DOWN   = buttons.BTN_DOWN
JOY_RIGHT  = buttons.BTN_RIGHT
JOY_UP     = buttons.BTN_UP
JOY_LEFT   = buttons.BTN_LEFT
BTN_HOME   = buttons.BTN_HOME

def input_attach(button, callback=None):
	global activeList, listUpCallback, listDownCallback
	if button < 0 or button > 7:
		return
	doAttach = True
	if button == JOY_UP:
		listUpCallback = callback
		if activeList:
			doAttach = False
	elif button == JOY_DOWN:
		listDownCallback = callback
		if activeList:
			doAttach = False
	if doAttach:
		buttons.attach(button, callback)


_internalListUpCallback = None
_internalListDownCallback = None

def _internalGlobalUpCallback(pressed):
	global _internalListUpCallback
	if _internalListUpCallback:
		_internalListUpCallback(pressed)

def _internalGlobalDownCallback(pressed):
	global _internalListDownCallback
	if _internalListDownCallback:
		_internalListDownCallback(pressed)

def _inputAttach(button, realCallback,thisList=None):
	global _internalListUpCallback, _internalListDownCallback, activeList
	activeList = thisList
	if button == JOY_UP:
		_internalListUpCallback = realCallback
		buttons.attach(JOY_UP, _internalGlobalUpCallback)
	if button == JOY_DOWN:
		_internalListDownCallback = realCallback
		buttons.attach(JOY_DOWN, _internalGlobalDownCallback)

LUT_NORMAL = display.FLAG_LUT_NORMAL
LUT_FASTER = display.FLAG_LUT_FAST
LUT_FASTEST = display.FLAG_LUT_FASTEST
LUT_FULL   = display.FLAG_LUT_GREYSCALE #display.FLAG_FULL
GREYSCALE = display.FLAG_LUT_GREYSCALE

BLACK = 0x000000
WHITE = 0xFFFFFF

def string(x,y,text,font,color):
	if font == "Roboto_pixelade13":
		font = "roboto_regular18"
	if font == "fixed_10x20":
		font = "roboto_regular18"
	if not color:
		color = 0
	if font:
		display.drawText(x, y, text, color, font)
	else:
		display.drawText(x, y, text, color)

def width():
	return display.width()

def height():
	return display.height()

def pixel(x,y,color):
	display.drawPixel(x,y,color)

def display_image(x,y,data):
	display.drawPng(x,y,data)

def fill_circle(x, y, r, color):
	display.drawCircle(x, y, r, 0, 359, True, color)

def circle(x, y, r, color):
	display.drawCircle(x, y, r, 0, 359, False, color)

def clear(arg=None):
	if arg != None:
		display.drawFill(arg)
	else:
		display.drawFill()

def line(x0, y0, x1, y1, color):
	display.drawLine(x0, y0, x1, y1, color)

def flush(arg=None):
	global display_flags
	
	if arg == 0xff:
		arg = display.FLAG_LUT_GREYSCALE

	if arg == None and display_flags == 0:
		display.flush(display.FLAG_LUT_FAST)
	elif arg != None:
		display.flush(arg)
	else:
		display.flush(display_flags)
	
def get_string_width(text, font=None):
	if font:
		return display.getTextWidth(text, font)
	else:
		return display.getTextWidth(text)

def get_string_height(text, font=None):
	if font:
		return display.getTextHeight(text, font)
	else:
		return display.getTextHeight(text)

def init():
	print("This app uses the UGFX compatibility layer, using UGFX is deprecated, please check the wiki for details!")

def input_init():
	pass

def set_lut(arg):
	display_flags = arg

justifyLeft = 0
justifyCenter = 1
justifyRight = 2
		
def area(x,y,w,h,color):
	display.drawRect(x,y,w,h,True,color)
	
def rounded_box(x,y,w,h,r,color):
	display.drawLine(x+r,   y,     x+w-1-r, y,       color)
	display.drawLine(x+r,   y+h-1, x+w-1-r, y+h-1,   color)
	display.drawLine(x,     y+r,   x,       y+h-1-r, color)
	display.drawLine(x+w-1, y+r,   x+w-1,   y+h-1-r, color)
	display.drawCircle(x+r,     y+r,     r, 270, 359, False, color)
	display.drawCircle(x+w-1-r, y+r,     r, 0,   90,  False, color)
	display.drawCircle(x+r,     y+h-1-r, r, 180, 270, False, color)
	display.drawCircle(x+w-1-r, y+h-1-r, r, 90,  180, False, color)
	#display.drawRect(x,y,w,h,False,color)

def fill_rounded_box(x,y,w,h,r,color):
	display.drawRect(x,y,w,h,True,color)

def string_box(x,y,w,h,text,font,color,align,testMode=False,newHeight=0):
	lines = wordWrap(text, w, font).split("\n")
	if len(lines) < 1:
		return
	
	for i in range(len(lines)):
		textWidth  = display.getTextWidth(lines[i], font)
		textHeight = display.getTextHeight(lines[0], font)
		text_x = x + w//2 - textWidth//2
		text_y = y + h//2 - textHeight//2 + textHeight*i - (textHeight*(len(lines)-1))//2
		if testMode:
			print("Components", y, h//2, textHeight//2, textHeight*i, (textHeight*(len(lines)-1))//2)
			print("Line",i,lines[i],text_x, text_y)
		display.drawText(text_x, text_y, lines[i], color, font)
		if testMode:
			display.drawRect(text_x, text_y, textWidth, textHeight, 0, 0)
			display.drawRect(text_x, text_y, textWidth, newHeight, 0, 0)
		

# Listbox UI element
class List():
	def __init__(self, x, y, w, h):
		self.x = x
		self.y = y
		self.w = w
		self.h = h
		self.items = []
		self.selected = 0
		global activeList
		activeList = self
		self.lines = self.h // (display.getTextHeight(" ", "roboto_regular18")+6)
		self.offset = 0
		self.visible(True)
		self.enabled(True)
	
	def _draw(self):
		if self._visible:
			display.drawRect(self.x, self.y, self.w, self.h, True, 0xFFFFFF)
			display.drawRect(self.x, self.y, self.w, self.h, False, 0x000000)
			cursor = (self.x+1,self.y+1)
			totalHeight = 0
			for i in range(len(self.items)-self.offset):
				cursor = (self.x+1,cursor[1])
				item = self.items[self.offset+i]
				lineHeight = display.getTextHeight(item, "roboto_regular18")
				
				while display.getTextWidth(item, "roboto_regular18") > self.w:
					item = item[:-1]
				
				totalHeight += lineHeight+6
				if totalHeight >= self.h:
					break
				color = 0x000000
				if self.offset+i == self.selected:
					display.drawRect(self.x, cursor[1], self.w, lineHeight+6, True, 0x000000)
					color = 0xFFFFFF
				cursor = (self.x,cursor[1]+3)
				display.drawText(cursor[0]+2, cursor[1], item+"\n", color, "roboto_regular18")
				cursor = (self.x,cursor[1]+3+display.getTextHeight(item+"\n", "roboto_regular18"))
	
	def add_item(self, caption):
		if type(caption) == type(""):
			i = self.items.append(caption)
		elif type(caption) == type(b""):
			i = self.items.append(caption.decode('utf-8'))
		else:
			i = self.items.append(str(caption))
		if self._enabled:
			self._draw()
		return i
	
	def count(self):
		return len(self.items)
	
	def selected_text(self):
		return self.items[self.selected]
	
	def remove_item(self, pos):
		self.items.pop(pos)
		if self.selected >= pos and self.selected > 0:
			self.selected -= 1
		if self._enabled:
			self._draw()
		#print("Remove item from pos", pos,"selected",self.selected)
	
	def selected_index(self, setValue=None):
		if setValue:
			self.selected = setValue
			if self.selected < self.offset:
				self.offset = self.selected
			if self.selected >= self.offset+self.lines:
				self.offset = self.selected+self.lines-1
			self._draw()
		else:
			return self.selected
	
	def destroy(self):
		self.items = []
		self.selected = 0
		activeList = None
		_inputAttach(JOY_UP, listUpCallback, self)
		_inputAttach(JOY_DOWN, listDownCallback, self)
		
	def _onUp(self, pressed):
		global listUpCallback
		if (pressed):
			if self.selected > 0:
				self.selected-=1
				if self.selected < self.offset:
					self.offset = self.selected
			self._draw()
		if listUpCallback:
			listUpCallback(pressed)
			
	def _onDown(self, pressed):
		global listDownCallback
		if (pressed):
			if self.selected < len(self.items)-1:
				self.selected+=1
				if self.selected >= self.offset+self.lines:
					self.offset += 1
				#print("onDown", self.selected, len(self.items), self.offset, self.items[self.selected])
			self._draw()
		if listDownCallback:
			listDownCallback(pressed)
	
	def visible(self, arg):
		self._visible = arg
		self._draw()

	def enabled(self, val):
		self._enabled = val
		global activeList, listUpCallback, listDownCallback
		if self._enabled:
			activeList = self
			_inputAttach(JOY_UP, self._onUp, self)
			_inputAttach(JOY_DOWN, self._onDown, self)
		else:
			activeList = None
			_inputAttach(JOY_UP, listUpCallback, self)
			_inputAttach(JOY_DOWN, listDownCallback, self)
	
	def clear(self):
		while len(self.items): #Keep the same list!
			self.items.pop()
		self.offset = 0
		self.selected = 0
		gc.collect()
			
def wordWrap(message, width=None, font="7x5"):
	message = message.split("\n")
	lines = []
	line = ""
    
	if width==None:
		width=display.width()
    
	for messagePart in message:
		if line != "":
			lines.append(line)
			line = ""
		words = messagePart.split(" ")
		for word in words:
			wordLength = display.getTextWidth(word, font)
			lineLength = display.getTextWidth(line, font)
			if wordLength > width:
				lines.append(line)
				lines.append(word)
				line = ""
			elif lineLength+wordLength < width:
				if (line==""):
					line = word
				else:
					line += " "+word
			else:
				lines.append(line)
				line = word
	if len(line) > 0:
		lines.append(line)
	return "\n".join(lines)

def box(x,y,width,height,color):
	display.drawRect(x,y,width,height,False,color)
