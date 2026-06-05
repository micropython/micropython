import display, orientation, term, term_menu, sys, ujson, system, buttons, machine, os, time

default_icon = b'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00 \x00\x00\x00 \x01\x00\x00\x00\x00[\x01GY\x00\x00\x00nIDAT\x08\xd7U\xce\xb1\r\x800\x0c\x05\xd1\x1fQ\xa4\xcc\x02\x88\xacAEVb\x02\xcchH\x14\xac\x01b\x00R\xa6\xb0l\x1cRq\xc5\xab\x0f\xf8\n\xd9 \x01\x9c\xf8\x15]q\x1b|\xc6\x89p\x97\x19\xf1\x90\x11\xf11\x92j\xdf \xd5\xe1\x87L\x06W\xf2b\\b\xec\x15_\x89\x82$\x89\x91\x98\x18\x91\xa94\x82j\x86g:\xd11mpLk\xdbhC\xd6\x0b\xf2 ER-k\xcb\xc6\x00\x00\x00\x00IEND\xaeB`\x82'

trash_icon = b'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00 \x00\x00\x00 \x01\x03\x00\x00\x00I\xb4\xe8\xb7\x00\x00\x00\x06PLTE\xff\xff\xff\x00\x00\x00U\xc2\xd3~\x00\x00\x00\tpHYs\x00\x00\x13\xaf\x00\x00\x13\xaf\x01c\xe6\x8e\xc3\x00\x00\x00\x19tEXtSoftware\x00www.inkscape.org\x9b\xee<\x1a\x00\x00\x00VIDAT\x08\x99\x95\xce\xb1\r\x80 \x14\x84\xe13\x16\x8c\xc1(\x8c&\x1b\xbc\x91t\x03F\x80\x15\x08\r\t\xe8y\x89\x95\xb1\xf2/\xbe\xf2r\xa0\x02\xbb\x17\xc5\x89c\x15\xa9\t\xab\xc4iuG\x01\xdcC\xcf\xc3\xa3\x93/F\x16\xd5~\xd0\xd2ge\x92\x01\x13\x08\xb8\x80\r\x8c\x8b\x1e\xa8\x1bL\xedW\xf4`=\x10\x0f\x00\x00\x00\x00IEND\xaeB`\x82'

def drawTitle():
	display.drawFill(0x000000)
	display.drawRect(0,0,display.width(), 10, True, 0xFFFFFF)
	display.drawText(2, 0, "SD card tool", 0x000000, "org18")

def drawMessageBox(text):
	print(text)
	width = display.getTextWidth(text, "org18")
	height = display.getTextHeight(text, "org18")
	display.drawRect((display.width()-width-4)//2, (display.height()-height-4)//2, width+2, height+2, True, 0xFFFFFF)
	display.drawText((display.width()-width)//2, (display.height()-height)//2-2, text, 0x000000, "org18")

def drawApp(app, position, amount):
	drawTitle()
	positionText = "{}/{}".format(position+1, amount)
	if app["path"].startswith("/sd"):
		positionText  = "SD "+positionText
	positionWidth = display.getTextWidth(positionText, "org18")
	display.drawText(display.width()-positionWidth-2, 0, positionText, 0x000000, "org18")
	titleWidth = display.getTextWidth(app["name"], "7x5")
	display.drawText((display.width()-titleWidth)//2,display.height()-10, app["name"], 0xFFFFFF, "7x5")
	try:
		icon_data = None
		if app["icon"]:
			if not app["icon"].startswith(b"\x89PNG"):
				with open(app["path"]+"/"+app["icon"], "rb") as icon_file:
					icon_data = icon_file.read()
			else:
				icon_data = app["icon"]
		if not icon_data:
			display.drawPng(48,15,default_icon)
		else:
			info = display.pngInfo(icon_data)
			if info[0] == 32 and info[1] == 32:
				display.drawPng(48,15,icon_data)
			else:
				drawMessageBox("Invalid icon size\nExpected 32x32!")
	except BaseException as e:
		sys.print_exception(e)
		drawMessageBox("Icon parsing error!")
	
	if not position < 1:
		display.drawText(0, display.height()//2-12, "<", 0xFFFFFF, "roboto_regular18")	
	if not position >= (amount-1):
		display.drawText(display.width()-10, display.height()//2-12, ">", 0xFFFFFF, "roboto_regular18")
	
	display.flush()

def loadInfo(folder, name):
	try:
		info_file = "{}/{}/metadata.json".format(folder, name)
		with open(info_file) as f:
			information = f.read()
		return ujson.loads(information)
	except BaseException as e:
		sys.print_exception(e)
		return {}

def listApps():
	apps = []
	for folder in sys.path:
		if folder != '':
			try:
				files = os.listdir(folder)
			except OSError:
				files = []
			for name in files:
				hidden = False
				app = {"path":folder+"/"+name, "name":name, "icon":None, "category":"unknown"}
				metadata = loadInfo(folder, name)
				if metadata:
					if "name" in metadata:
						app["name"]     = metadata["name"]
					if "category" in metadata:
						app["category"] = metadata["category"]
					if "icon" in metadata:
						app["icon"] = metadata["icon"]
					if "hidden" in metadata:
						hidden = metadata["hidden"]
				if not hidden:
					apps.append(app)
	return apps

def onLeft(pressed):
	global currentApp, apps
	if pressed:
		currentApp -= 1
		if currentApp < 0:
			currentApp = len(apps)-1
		drawApp(apps[currentApp], currentApp, len(apps))

def onRight(pressed):
	global currentApp, apps
	if pressed:
		currentApp += 1
		if currentApp >= len(apps):
			currentApp = 0
		drawApp(apps[currentApp], currentApp, len(apps))

def uninstall(path):
	print("Uninstall", path)
	flist = os.listdir(path)
	print("files", flist)
	for i in range(len(flist)):
		f = flist[i]
		display.drawFill(0x000000)
		display.drawRect(0,0,display.width(), 10, True, 0xFFFFFF)
		display.drawText(2, 0, "UNINSTALL", 0x000000, "org18")
		drawMessageBox("Removing file...\n{}".format(f))
		display.flush()
		os.remove(path+"/"+f)
	display.drawFill(0x000000)
	display.drawRect(0,0,display.width(), 10, True, 0xFFFFFF)
	display.drawText(2, 0, "UNINSTALL", 0x000000, "org18")
	drawMessageBox("Removing folder...")
	display.flush()
	os.rmdir(path)
	display.drawFill(0x000000)
	display.drawRect(0,0,display.width(), 10, True, 0xFFFFFF)
	display.drawText(2, 0, "UNINSTALL", 0x000000, "org18")
	drawMessageBox("App removed!")
	display.flush()

def move(path):
	if path.startswith("/sd"):
		target = path[3:] #Remove /sd from the start
	else:
		target = "/sd"+path #Add /sd to the start
	
	print("Moving", path, "to", target)
	
	drawTitle()
	drawMessageBox("Creating folder...")
	display.flush()
	
	try:
		os.mkdir("/sd/lib")
	except:
		pass
	
	try:
		os.mkdir("/sd/apps")
	except:
		pass
	
	try:
		try:
			os.mkdir(target)
		except:
			uninstall(target)
			os.mkdir(target)
	except:
		drawTitle()
		drawMessageBox("FAILED TO\nCREATE TARGET")
		display.flush()
		time.sleep(4)
		showMenu()
		return
	
	flist = os.listdir(path)
	
	for i in range(len(flist)):
		f = flist[i]
		drawTitle()
		drawMessageBox("Copying {}...".format(f))
		display.flush()
		block = 1
		src = open(path+"/"+f, "rb")
		tgt = open(target+"/"+f, "wb")
		while True:
			drawTitle()
			drawMessageBox("Copying {}...\nBlock #{}".format(f, block))
			display.flush()
			data = src.read(1024)
			if len(data) < 1:
				break
			tgt.write(data)
			block+=1
		src.close()
		tgt.close()
	
	for i in range(len(flist)):
		f = flist[i]
		drawTitle()
		drawMessageBox("Removing file...\n{}".format(f))
		display.flush()
		os.remove(path+"/"+f)
	drawTitle()
	drawMessageBox("Removing folder...")
	display.flush()
	os.rmdir(path)
	drawTitle()
	drawMessageBox("App moved!")
	display.flush()
	time.sleep(2)	
	showMenu()

def onA(pressed):
	global currentApp, apps
	if pressed:
		display.drawFill(0x000000)
		drawMessageBox("Moving app...")
		display.flush()
		move(apps[currentApp]["path"])

def onB(pressed):
	if pressed:
		display.drawFill(0x000000)
		drawMessageBox("Starting launcher...")
		display.flush()
		system.launcher()

# Launcher
orientation.landscape()
drawTitle()
drawMessageBox("Loading...")
display.flush()
term.header(True, "Loading...")

try:
	os.listdir("/sd")
except:
	drawTitle()
	drawMessageBox("NO SD CARD FOUND\nInsert SD card!")
	display.flush()
	time.sleep(5)
	system.launcher()

buttons.attach(buttons.BTN_LEFT,  onLeft)
buttons.attach(buttons.BTN_RIGHT, onRight)
buttons.attach(buttons.BTN_A,     onA)
try:
	buttons.attach(buttons.BTN_START, onB)
except:
	pass
buttons.attach(buttons.BTN_B,     onB)

def showMenu():
	global currentApp, apps
	apps = listApps()
	currentApp = 0
	drawApp(apps[0],0,len(apps))

showMenu()
