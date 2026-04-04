import term, orientation, system, time

system.serialWarning()

def getOrientationString(currentValue):
	orientationString = str(currentValue)
	if currentValue == 0:
		orientationString = "Landscape"
	if currentValue == 90:
		orientationString = "Portrait"
	if currentValue == 180:
		orientationString = "Landscape inverted"
	if currentValue == 270:
		orientationString = "Portrait inverted"
	return orientationString

items = ["Landscape", "Portrait", "Landscape inverted", "Portrait inverted", "< Back"]
options = [0, 90, 180, 270, -1, -1]
currentValue = orientation.getDefault()
orientationString = getOrientationString(currentValue)
message = "Current orientation: "+orientationString+"\n"
newValue = options[term.menu("Configure orientation", items, 0, message)]
if newValue < 0:
	system.home(True)
term.header(True, "Configure orientation")
if orientation.setDefault(newValue):
	print("Default orientation changed to "+getOrientationString(newValue))
else:
	print("Default orientation could not be changed to "+getOrientationString(newValue))
time.sleep(1)
system.home(True)
