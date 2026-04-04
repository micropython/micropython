# Device specific system functions

def configureWakeupSource():
	#machine.RTC().wake_on_ext0(pin = machine.Pin(34), level = 0)
	#machine.RTC().wake_on_ext1([machine.Pin(5, machine.Pin.IN, machine.Pin.PULL_UP)], 0)
	return False # Not supported

def prepareForSleep():
	pass

def prepareForWakeup():
	pass

def showLoadingScreen(app=""):
	pass # Not supported

def showMessage(message="", icon=None):
	pass # Not supported
