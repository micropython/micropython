import machine, time, os, term
import _device as device
import rtcmem

def reboot():
	device.prepareForSleep()
	machine.deepsleep(1)

def sleep(duration=0, status=False):
	if not device.configureWakeupSource():
		print("No wakeup source available, rebooting...")
		reboot()
		return
	if (duration >= 86400000): #One day
		duration = 0
	if status:
		if duration < 1:
			term.header(True, "Sleeping until a touch button is pressed!")
		else:
			term.header(True, "Sleeping for "+str(duration)+"ms...")
	device.prepareForSleep()
	machine.deepsleep(duration)

def start(app, status=False):
	"""
	if status:
		if app == "" or app == "launcher":
			term.header(True, "Loading menu...")
		else:
			term.header(True, "Loading application "+app+"...")
		device.showLoadingScreen(app)
	"""
	rtcmem.write_string(app)
	reboot()

def home(status=False):
	start("", status)

def launcher(status=False):
	start("dashboard.launcher", status)

def shell(status=False):
	start("shell", status)

# Over-the-air updating

def ota(status=False):
	"""
	if status:
		term.header(True, "Starting update...")
		device.showLoadingScreen("OTA update")
	"""
	rtcmem.write(0,1) # Boot mode selection magic
	rtcmem.write(1,254)
	reboot()

def eraseStorage():
	rtcmem.write(0,2)
	rtcmem.write(1,253)
	reboot()

def serialWarning():
	device.showMessage("This app can only be controlled using the USB serial port!", "/media/usb.png")
	
def crashedWarning():
	device.showMessage("FATAL ERROR\nthe app has crashed", "/media/alert.png")

def isColdBoot():
	if machine.wake_reason() == (7, 0):
		return True
	return False

def isWakeup(fromTimer=True, fromExt0=True, fromExt1=True, fromUlp=True):
	if fromExt0   and machine.wake_reason() == (3, 1):
		return True
	if fromExt1   and machine.wake_reason() == (3, 2):
		return True
	if fromTimer  and machine.wake_reason() == (3, 4):
		return True
	if fromUlp    and machine.wake_reason() == (3, 5):
		return True
	return False

__current_app__ = None

def currentApp():
	return __current_app__
