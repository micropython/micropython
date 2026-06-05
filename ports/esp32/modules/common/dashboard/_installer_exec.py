import wifi, woezel, gc, time, system

woezel.install_path = "/apps/python"

def showMessage(msg):
	import term, easydraw
	term.header(True, "Installer")
	print(msg)
	easydraw.messageCentered(msg, False)

showMessage("Connecting to WiFi...")

wifi.connect()

if not wifi.wait():
	showMessage("Could not connect to WiFi!")
	time.sleep(2)
	system.start("dashboard.installer")

showMessage("Preparing installation...")

with open("/cache/installList") as f:
	apps = f.read()

for app in apps.split("\n"):
	showMessage("Installing {}...".format(app))
	try:
		gc.collect()
		woezel.install(app)
	except woezel.LatestInstalledError:
		pass
	except BaseException as e:
		showMessage("Failed!")
		time.sleep(2)
		system.start("dashboard.installer")

with open("/cache/installList", "w") as f:
	f.write("")

showMessage("App installed!")
time.sleep(3)
system.start("dashboard.installer")
