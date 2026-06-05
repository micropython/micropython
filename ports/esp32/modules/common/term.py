import sys, consts, machine, time, uselect

def goto(x,y):
	sys.stdout.write("\033["+str(y)+";"+str(x)+"H")

def home():
	goto(1,1)

def clear():
	sys.stdout.write('\033[2J\033[3J\033[1;1H')
	home()
	
def color(fg=37, bg=40, style=0):
	sys.stdout.write("\033["+str(style)+";"+str(fg)+";"+str(bg)+"m")
	
def header(cls = False, text = ""):
	if cls:
		clear()
	else:
		home()
	if text:
		text = consts.INFO_HARDWARE_NAME+" - "+text
	else:
		text = consts.INFO_HARDWARE_NAME
	color(37, 44, 1)
	print(" "+text+u"\r\n")
	color()
	
def draw_menu_item(text, selected, width=32):
	space = ""
	if width-len(text) > 0:
		space = " "*(width-len(text))
	if (selected):
		color(30, 47, 0)
		print("> " + text+space)
	else:
		color()
		print("  " + text+space)

def draw_menu(title, items, selected=0, text="", width=32):
	header(False, title)
	if len(text)>0:
		print(text+"\n")
	for i in range(0, len(items)):
		draw_menu_item(items[i], selected==i, width)
	color()
	
def calculate_offset(text):
	offset = 3
	if len(text):
		offset += 2
		for i in text:
			if i == '\n':
				offset +=1
	return offset
	
def draw_menu_partial(title, items, selected=0, text="", width=32,lastSelected=0):
	offset = calculate_offset(text)
	if selected != lastSelected:
		goto(1,offset+lastSelected)
		draw_menu_item(items[lastSelected], False, width)
		goto(1,offset+selected)
		draw_menu_item(items[selected], True, width)
		goto(1,offset+selected)
		color()

def flush_stdin():
	key = 1
	while key is not None:
		list = uselect.select([sys.stdin], [], [], 0.01)
		if list[0]:
			key = sys.stdin.read(1)
		else:
			key = None	

def menu(title, items, selected = 0, text="", width=32):
	clear()
	lastSelected = selected
	needFullDraw = True
	while True:
		if needFullDraw:
			draw_menu(title, items, selected, text, width)
		else:
			draw_menu_partial(title, items, selected, text, width, lastSelected)
		lastSelected = selected
		key = sys.stdin.read(1)
		feedPm()
		try:
			if key == "\x03" or key == "\x04": # CTRL+C or CTRL+D
				import shell
			if (ord(key)==0x1b):
				key = sys.stdin.read(1)
				if (key=="["):
					key = sys.stdin.read(1)
					if (key=="A"):
						if (selected>0):
							selected -= 1
							needFullDraw = False
					if (key=="B"):
						if (selected<len(items)-1):
							selected += 1
							needFullDraw = False
			elif (ord(key)==0x01):
				import tasks.powermanagement as pm
				pm.disable()
				draw_menu(title, items, selected, text)
				pm.resume()
				
			elif (key == "\n" or key == "\r"):
				flush_stdin()
				return selected
			else:
				clear()
				needFullDraw = True #Refresh on any other key
		except BaseException as e: # Input parser crashed for some reason
			clear()
			needFullDraw = True
			sys.print_exception(e)
			time.sleep(2)
			flush_stdin()

def prompt(prompt, x, y, buff = ""):
	running = True
	while running:
		goto(x, y)
		sys.stdout.write(prompt+": ")
		color(30, 47, 0)
		sys.stdout.write(buff)
		if len(buff) < 64:
			sys.stdout.write(" "*(64-len(buff)))
		color()
		last = sys.stdin.read(1)
		if last == '\n' or last == '\r':
			return buff
		if ord(last) >= 32 and ord(last) < 127:
			buff += last
		if ord(last) == 127:
			buff = buff[:-1]

def empty_lines(count = 10):
	for i in range(0,count):
		print("")

# Functions for feeding the power management task

powerManagement = None

def setPowerManagement(pm):
	global powerManagement
	powerManagement = pm
	
def feedPm():
	if powerManagement != None:
		powerManagement.set_timeout(300000) #Set timeout to 5 minutes
		powerManagement.feed(False)
