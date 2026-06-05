import display, version, time, orientation, machine

lastTitle = ""

# Functions
def msg_nosplit(message, title = 'Loading...', reset = False):
	"""Show a terminal style loading screen with title

	title can be optionaly set when resetting or first call
	"""
	global messageHistory, lastTitle
	
	num_lines = ((display.height() - 11)//display.getTextHeight(" ", "7x5"))
	
	if reset:
		lastTitle = title
	
	try:
		messageHistory
		if reset:
			raise exception
	except:
		display.drawFill(0x000000)
		messageHistory = []
		lastTitle = title
		
	lineHeight = display.getTextHeight(" ", "7x5")

	if len(messageHistory)<num_lines:
		display.drawText(0, 11 + (len(messageHistory) * lineHeight), message, 0xFFFFFF, "7x5")
		messageHistory.append(message)
	else:
		messageHistory.pop(0)
		messageHistory.append(message)
		display.drawRect(0, 11, display.width(), display.height()-15, True, 0x000000)
		for i, message in enumerate(messageHistory):
			display.drawText(0, 11 + (i * lineHeight), message, 0xFFFFFF, "7x5")

	display.drawRect(0, 0, display.width(), 10, True, 0xFFFFFF)
	display.drawText(2, 0, lastTitle, 0x000000, "org18")

	display.flush(display.FLAG_LUT_FASTEST)
	
	return len(messageHistory)>=num_lines-1
	
def msg(message, title = "Loading...", reset = False, wait = 0):
	num_lines = ((display.height() - 11)//display.getTextHeight(" ", "7x5"))
	scroll = False
	try:
		lines = lineSplit(str(message))
		for i in range(len(lines)):
			if i > 0:
				scroll = msg_nosplit(lines[i])
			else:
				scroll = msg_nosplit(lines[i], title, reset)
			if (i > 1) and (wait > 0):
				time.sleep(wait)
	except BaseException as e:
		print("!!! Exception in easydraw.msg !!!")
		print(e)
	return scroll

def lineCentered(pos_y, line, font, color):
	if font:
		width = display.getTextWidth(line, font)
	else:
		width = display.getTextWidth(line)
	pos_x = int((display.width()-width) / 2)
	if font:
		display.drawText(pos_x, pos_y, line, color, font)
	else:
		display.drawText(pos_x, pos_y, line, color)

def messageCentered(message, firstLineTitle=True, png=None):
	try:
		display.drawFill(0x000000)
		color = 0xFFFFFF
		font1 = "Roboto_Regular18"
		font2 = "Roboto_Regular12"
		font1Height = display.getTextHeight(" ", font1)
		font2Height = display.getTextHeight(" ", font2)
		
		message = message.split("\n")
		lines = []
		for i in range(len(message)):
			line = message[i]
			if len(line) < 1:
				lines.append("")
			else:
				if firstLineTitle and i == 0:
					lines.extend(lineSplit(line, display.width(), font1))
				else:
					lines.extend(lineSplit(line, display.width(), font2))
		
		pngSize = (0,0)
		if png != None:
			try:
				pngSize = display.pngInfo(png)
				pngSize = [pngSize[0], pngSize[1]+2] #Little bit of extra offset
			except BaseException as e:
				#print("Error in PNG height",e)
				pass
		
		textHeight = len(lines)*font2Height
		
		if firstLineTitle:
			textHeight -= font2Height
			textHeight += font1Height
		
		offset_y = (display.height()-pngSize[1]-textHeight)//2
		
		if png != None:
			try:
				display.drawPng((display.width()-pngSize[0])//2, offset_y, png)
				offset_y += pngSize[1]
			except BaseException as e:
				print("Error in PNG draw",e)
		
		for i in range(len(lines)):
			if firstLineTitle and i == 0:
				lineCentered(offset_y, lines[i], font1, color)
				offset_y += font1Height
			else:
				lineCentered(offset_y, lines[i], font2, color)
				offset_y += font2Height
		
		display.flush(display.FLAG_LUT_FASTEST)
	except BaseException as e:
		print("Error in messageCentered!",e)

def nickname(y = 5, font = "Roboto_Regular18", color = 0x000000, unusedParameter=None):
	nick = machine.nvs_getstr("owner", "name")
	if not nick:
		return y
	lines = lineSplit(nick, display.width(), font)
	for i in range(len(lines)):
		line = lines[len(lines)-i-1]
		pos_x = int((display.width()-display.getTextWidth(line, font)) / 2)
		lineHeight = display.getTextHeight(line, font)
		if font:
			display.drawText(pos_x, y+lineHeight*(len(lines)-i-1), line, color, font)
		else:
			display.drawText(pos_x, y+lineHeight*(len(lines)-i-1), line, color)
	return len(lines) * lineHeight

def battery(on_usb, vBatt, charging):
	pass

def disp_string_right_bottom(y, s, font="Roboto_Regular12"):
	l = display.getTextWidth(s, font)
	display.drawText(display.width()-l, display.height()-(y+1)*14, s, 0x000000, font)

def lineSplit(message, width=None, font=version.font_default):
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
	return lines
