import display, buttons

shift = 0  # 0 = lower, 1 = upper, 2 = caps, 3 = numbers, 4 = symbols
mode = 0  # 0 = keyboard, 1 = select, 2 = buttons

keyFont = "roboto_regular12"
textFont = "roboto_regular18"
yOffset = display.height() - 20 * 3
charWidth = display.getTextWidth("X", keyFont)
charHeight = display.getTextHeight("X", keyFont)
charOffsetX = int((20 - charWidth))
charOffsetXDouble = int((20 - charWidth * 1.5))
charOffsetY = 5

charMap = []
charMap.append(["q", "w", "e", "r", "t", "y", "u", "i", "o", "p"])
charMap.append([" ", "a", "s", "d", "f", "g", "h", "j", "k", "l"])
charMap.append(["SHIFT", " ", "z", "x", "c", "v", "b", "n", "m", "<<"])
# Uppercase
charMap.append(["Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P"])
charMap.append([" ", "A", "S", "D", "F", "G", "H", "J", "K", "L"])
charMap.append(["CAPS", " ", "Z", "X", "C", "V", "B", "N", "M", "<<"])
# Caps-lock
charMap.append(["Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P"])
charMap.append([" ", "A", "S", "D", "F", "G", "H", "J", "K", "L"])
charMap.append(["1 2 3", " ", "Z", "X", "C", "V", "B", "N", "M", "<<"])
# Numbers
charMap.append(["1", "2", "3", "4", "5", "6", "7", "8", "9", "0"])
charMap.append([" ", "@", "#", "$", "%", "^", "&", "*", "(", ")"])
charMap.append(["!@#$", " ", ".", ",", "=", "+", "[", "]", "?", "<<"])
# Numbers 2
charMap.append(["{", "}", ";", ":", '"', "'", ",", ".", "<", ">"])
charMap.append(["ENTER", " ", "~", "`", "/", "\\", ":)", ":(", " ", " "])
charMap.append(["A B C", " ", "-", "_", " ", " ", " ", " ", " ", "<<"])


def _correctLocation(right=False):
	global cx, cy
	if cx < 0:
		cx = 9
	if cy < 0:
		cy = 0
	if cy >= 3:
		cy = 2
	if cx >= len(charMap[cy]):
		cx = 0
	if cy == 2 and cx == 1:
		if right:
			cx = 2
		else:
			cx = 0


def onSelect(pressed):
	if pressed:
		global mode
		mode += 1
		if mode > 2:
			mode = 0
		draw()

def onStart(pressed):
	pass

def onDown(pressed):
	if pressed:
		if mode == 0:
			global cy
			cy += 1
			_correctLocation()
		draw()


def onRight(pressed):
	if pressed:
		global cx, cursorPos
		if mode == 0:
			cx += 1
			_correctLocation(True)
		if mode == 1:
			cursorPos += 1
		draw()


def onUp(pressed):
	if pressed:
		global cy
		if mode == 0:
			cy -= 1
			_correctLocation()
		draw()


def onLeft(pressed):
	if pressed:
		global cx, cursorPos
		if mode == 0:
			cx -= 1
			_correctLocation()
		if mode == 1:
			cursorPos -= 1
		draw()

def onA(pressed):
	global text, shift, cursorPos, _cbAccept, _cbCancel
	if pressed:
		if mode == 0:
			if cy == 2 and cx == 0:
				shift += 1
				if shift > 4:
					shift = 0
			elif cy == 2 and cx == len(charMap[2]) - 1:
				# Backspace key
				if len(text) > 0:
					# text = text[:-1]
					if len(text) > cursorPos and cursorPos > 0:
						text = text[0 : cursorPos - 1 :] + text[cursorPos::]
						cursorPos -= 1
					else:
						text = text[:-1]
						cursorPos -= 1
			else:
				offset = shift * 3
				if cursorPos >= len(text):
					cursorPos += 1
				if charMap[cy + offset][cx] == "ENTER":
					text += "\n"
				else:
					text += charMap[cy + offset][cx]
				if shift == 1:
					shift = 0
			_correctLocation()
			draw()
		if mode == 2:
			buttons.popMapping()
			_cbAccept(text)


def onB(pressed):
	global text, originalText, _cbAccept, _cbCancel
	if pressed and mode == 2:
		buttons.popMapping()
		if _cbCancel:
			_cbCancel(text)
		else:
			_cbAccept(originalText)


def draw():
	global cx, cy, text, cursorPos, title, yOffset, mode
	display.drawFill(0xFFFFFF)
	display.drawRect(0, yOffset - 12, display.width(), 12, True, 0x000000)
	display.drawRect(
		0, yOffset, display.width(), display.height() - yOffset, False, 0x000000
	)

	modeText = "keyboard"
	if mode == 1:
		modeText = "cursor"
	if mode == 2:
		modeText = "actions"

	display.drawText(0, yOffset - 12, "[SELECT] " + modeText, 0xFFFFFF, "Roboto_Regular12")
	display.drawRect(0, 0, display.width(), 14, True, 0)
	display.drawText(0, 0, title, 0xFFFFFF, "Roboto_Regular12")

	if cursorPos > len(text):
		cursorPos = len(text)
	if cursorPos < 0:
		cursorPos = 0

	textWithCursor = text[:cursorPos] + "|" + text[cursorPos:]

	textWithCursorLines = textWithCursor.split("\n")
	for i in range(len(textWithCursorLines)):
		display.drawText(0, 17 + i * 13, textWithCursorLines[i], 0x000000, textFont)

	for y in range(3):
		for x in range(10):
			xOffset = 0
			width = 29
			widthOffset = width
			if y == 1:
				xOffset = 6
			if y == 2 and x == 0:
				width *= 2
			if y == 2 and x == 1:
				continue
			if y == 1 and x == 0:
				width += xOffset
				xOffset = 0
			if y == 2 and x == 9:
				width += 6
			if x == 0 and y == 1 and shift == 4:
				width *= 2
			if x == 1 and y == 1 and shift == 4:
				width = 0
			selected = False
			if x == cx and y == cy:
				selected = True
			if width > 0:
				display.drawRect(
					x * widthOffset + xOffset,
					y * 20 + yOffset,
					width,
					20,
					True,
					0xFFFFFF,
				)
				display.drawRect(
					x * widthOffset + xOffset,
					y * 20 + yOffset,
					width,
					20,
					selected,
					0x000000,
				)
				color = 0
				if selected:
					color = 0xFFFFFF
				offset = shift * 3
				cxo = xOffset + charOffsetX
				# if x == 0 and y == 1:
				# 	cxo = xOffset+charOffsetXDouble
				display.drawText(
					x * widthOffset + cxo,
					y * 20 + yOffset + charOffsetY,
					charMap[y + offset][x],
					color,
					keyFont,
				)

	if mode == 2:
		display.drawRect(
			8,
			yOffset + 8,
			display.width() - 16,
			display.height() - yOffset - 16,
			True,
			0xFFFFFF,
		)
		display.drawRect(
			8,
			yOffset + 8,
			display.width() - 16,
			display.height() - yOffset - 16,
			False,
			0x000000,
		)
		display.drawText(
			12, yOffset + 12, "Press A to accept input", 0x000000, "Roboto_Regular12"
		)
		display.drawText(
			12, yOffset + 12 + 14, "Press B to cancel", 0x000000, "Roboto_Regular12"
		)

	display.flush(display.FLAG_LUT_FASTEST)


def show(newTitle, initialText, cbAccept, cbCancel=None):
	global cx, cy, text, originalText, title, cursorPos, _cbAccept, _cbCancel, mode
	mode = 0
	title = newTitle
	display.drawFill(0xFFFFFF)
	cx = 0
	cy = 0
	text = initialText
	originalText = initialText
	_cbAccept = cbAccept
	_cbCancel = cbCancel
	cursorPos = len(text)
	buttons.pushMapping();
	buttons.attach(buttons.BTN_A, onA)
	buttons.attach(buttons.BTN_B, onB)
	buttons.attach(buttons.BTN_SELECT, onSelect)
	buttons.attach(buttons.BTN_START, onStart)
	buttons.attach(buttons.BTN_DOWN, onDown)
	buttons.attach(buttons.BTN_RIGHT, onRight)
	buttons.attach(buttons.BTN_UP, onUp)
	buttons.attach(buttons.BTN_LEFT, onLeft)
	draw()
