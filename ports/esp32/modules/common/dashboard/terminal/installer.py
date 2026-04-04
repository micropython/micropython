import term, system, time, wifi, uos, sys, gc, woezel, uinstaller

system.serialWarning()

def showMessage(msg, error=False):
	term.header(True, "Installer")
	print(msg)

# Categories list
categories = woezel.get_categories()

def woezel_callback(text, error=False):
	print('[Woezel]: %s' % text)

def show_categories():
	system.serialWarning()
	opt = []
	for category in categories:
		opt.append(category["name"])
	opt.append("< Back to launcher")
	sel = term.menu("Installer - Categories", opt)
	if sel == len(categories):
		system.launcher(True)
	show_category(sel)

# Category browsing

def show_category(i):
	system.serialWarning()
	global category
	slug = categories[i]["slug"]
	name = categories[i]["name"]
	showMessage("Loading "+slug+"...")
	try:
		try:
			category = woezel.get_category(slug)
		except:
			showMessage("Failed to open category "+slug+"!", True)
			time.sleep(1)
			show_categories()
		gc.collect()
		opt = []
		for package in category:
			opt.append("%s rev. %s" % (package["name"], package["revision"]))
		opt.append("< Back to categories")
		sel = term.menu("Installer - "+name, opt)
		if sel == len(category):
			show_categories()
		install_app(sel,i)
	except BaseException as e:
		sys.print_exception(e)
		showMessage(e, True)
		time.sleep(1)
		show_categories()

# Install application

def install_app(i,fromcat):
	system.serialWarning()
	global category
	slug = category[i]["slug"]
	category = None
	gc.collect()
	if not wifi.status():
		wifi.connect()
		wifi.wait()
		if not wifi.status():
			showMessage("Unable to connect to WiFi.")
			time.sleep(2)
			show_category(fromcat)
	showMessage("Installing "+slug+"...")
	uinstaller.install(slug)

#Main application

showMessage("Loading categories...")
if not woezel.update_cache():
	# woezel.set_progress_callback(woezel_callback)
	showMessage("Failed to load repository. Returning to launcher...")
	system.launcher()

show_categories()
