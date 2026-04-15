import orientation, dashboard.resources.woezel_repo as woezel_repo, term, easydraw, system, time, gc, ugfx, wifi, uos, json, sys, woezel, display, errno, buttons, display

repo = woezel_repo

orientation.default()

def showMessage(msg, error=False, icon_wifi=False, icon_ok=False):
	term.header(True, "Installer")
	print(msg)
	easydraw.messageCentered(msg, False, None)

# Listbox element
myList = ugfx.List(0,28,ugfx.width(),ugfx.height()-28-28)

# Generic actions
def btn_unhandled(pressed):
	display.flush(display.FLAG_LUT_FASTEST)

def btn_exit(pressed):
	if pressed:
		system.launcher()

def btn_update(pressed):
	if pressed:
		repo.update()
		system.start("dashboard.installer", True)

# Categories list

def show_categories(pressed = True, fromAppInstall = False):
	if not pressed:
		return
	ugfx.input_attach(ugfx.JOY_UP, btn_unhandled)
	ugfx.input_attach(ugfx.JOY_DOWN, btn_unhandled)
	ugfx.input_attach(ugfx.JOY_LEFT, btn_unhandled)
	ugfx.input_attach(ugfx.JOY_RIGHT, btn_unhandled)
	ugfx.input_attach(ugfx.BTN_A, btn_unhandled)
	ugfx.input_attach(ugfx.BTN_B, btn_unhandled)
	display.drawFill(0xFFFFFF)
	display.drawText(0, 4, "Hatchery - Categories", 0x000000, "roboto_regular18")
	display.drawText(0, display.height() - 20, "[A] choose [B] back [H] exit [SE] update repo", 0x000000, "roboto_regular12")
	myList.clear()
	for category in repo.categories:
		myList.add_item("%s (%d) >" % (category["name"], category["eggs"]))
	myList.enabled(True)
	myList.visible(True)
	#Input handling
	ugfx.input_attach(ugfx.BTN_B, btn_unhandled)
	ugfx.input_attach(ugfx.JOY_UP, btn_unhandled)
	ugfx.input_attach(ugfx.JOY_DOWN, btn_unhandled)
	ugfx.input_attach(ugfx.JOY_LEFT, btn_unhandled)
	ugfx.input_attach(ugfx.JOY_RIGHT, btn_unhandled)
	try:
		buttons.BTN_START
		ugfx.input_attach(ugfx.BTN_START, btn_exit)
	except:
		ugfx.input_attach(ugfx.BTN_B, btn_exit)
	try:
		buttons.BTN_HOME
		ugfx.input_attach(ugfx.BTN_HOME, btn_exit)
	except:
		ugfx.input_attach(ugfx.BTN_B, btn_exit)
	try:
		buttons.BTN_SELECT
		ugfx.input_attach(ugfx.BTN_SELECT, btn_update)
	except:
		ugfx.input_attach(ugfx.JOY_UP, btn_update)
	ugfx.input_attach(ugfx.BTN_A, show_category)
	display.flush(display.FLAG_LUT_NORMAL)

# Category browsing

lastCategory = 0

def show_category(pressed=True, fromAppInstall = False):
	global lastCategory
	if not pressed:
		return
	ugfx.input_attach(ugfx.JOY_UP, btn_unhandled)
	ugfx.input_attach(ugfx.JOY_DOWN, btn_unhandled)
	ugfx.input_attach(ugfx.JOY_LEFT, btn_unhandled)
	ugfx.input_attach(ugfx.JOY_RIGHT, btn_unhandled)
	ugfx.input_attach(ugfx.BTN_A, btn_unhandled)
	ugfx.input_attach(ugfx.BTN_B, btn_unhandled)
	ugfx.clear(ugfx.WHITE)
	
	global category
	if not fromAppInstall:
		lastCategory = myList.selected_index()
	slug = repo.categories[lastCategory]["slug"]
	name = repo.categories[lastCategory]["name"]
	showMessage("Loading "+name+"...")
	display.drawFill(0xFFFFFF)
	display.drawText(0, display.height() - 20, "[A] choose [B] back [H] exit [SE] update repo", 0x000000, "roboto_regular12")
	display.drawText(0, 4, "Hatchery - " + name, 0x000000, "roboto_regular18")
	myList.clear()
	try:
		try:
			category = repo.getCategory(slug)
		except BaseException as e:
			showMessage("Failed to open category "+slug+"!", True)
			display.drawFill()
			time.sleep(2)
			show_categories()
			return
		print(category)
		gc.collect()
		for package in category:
			myList.add_item("%s rev. %s" % (package["name"], package["revision"]))
			print("Adding", "%s rev. %s" % (package["name"], package["revision"]))
		myList.visible(True)
		myList.enabled(True)
		try:
			ugfx.input_attach(ugfx.BTN_START, btn_exit)
		except:
			pass
		try:
			ugfx.input_attach(ugfx.BTN_HOME, btn_exit)
		except:
			pass
		try:
			ugfx.input_attach(ugfx.BTN_SELECT, btn_update)
		except:
			pass
		ugfx.input_attach(ugfx.BTN_A, install_app)
		ugfx.input_attach(ugfx.BTN_B, show_categories)
		ugfx.input_attach(ugfx.JOY_UP, btn_unhandled)
		ugfx.input_attach(ugfx.JOY_DOWN, btn_unhandled)
		ugfx.input_attach(ugfx.JOY_LEFT, btn_unhandled)
		ugfx.input_attach(ugfx.JOY_RIGHT, btn_unhandled)
		
		display.flush(display.FLAG_LUT_NORMAL)
	except BaseException as e:
		sys.print_exception(e)
		print("ERROR", e)
		showMessage("Internal error", True)
		display.drawFill()
		time.sleep(10)
		show_categories()
	print("Done!")

# Install application

def install_app(pressed=True):
	global category
	if pressed:
		slug = category[myList.selected_index()]["slug"]
		#Input handling
		try:
			ugfx.input_attach(ugfx.BTN_START, btn_exit)
		except:
			pass
		try:
			ugfx.input_attach(ugfx.BTN_SELECT, btn_unhandled)
		except:
			pass
		ugfx.input_attach(ugfx.BTN_A, btn_unhandled)
		ugfx.input_attach(ugfx.BTN_B, btn_unhandled)
		ugfx.input_attach(ugfx.JOY_UP, btn_unhandled)
		ugfx.input_attach(ugfx.JOY_DOWN, btn_unhandled)
		ugfx.input_attach(ugfx.JOY_LEFT, btn_unhandled)
		ugfx.input_attach(ugfx.JOY_RIGHT, btn_unhandled)
		category = []
		myList.clear()
		showMessage("Installing "+slug+"...")
		with open("/cache/installList", "w") as f:
			f.write(slug)
		system.start("dashboard._installer_exec")

#Main application

showMessage("Loading categories...")
display.drawFill()
if not repo.load():
	if not repo.update():
		if repo.lastUpdate==0:
			time.sleep(2)
			showMessage("Failed to load repository!")
			time.sleep(4)
			display.drawFill()
			system.launcher()

show_categories()
