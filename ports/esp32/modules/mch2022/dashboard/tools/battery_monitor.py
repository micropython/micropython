import display, time, neopixel, buttons, voltages

dataPoints = [0]*64
dataPoints2 = [0]*64


ledState = False
def toggleLeds(pressed):
	global ledState
	if pressed:
		ledState = not ledState
		if ledState:
			neopixel.send(bytes([0xFF, 0xFF, 0xFF]*12))
		else:
			neopixel.send(bytes([0, 0, 0]*12))

buttons.attach(buttons.BTN_A, toggleLeds)

def draw(vbatt, vusb):
	display.drawFill(0x000000)
	display.drawText(0,0,"Battery")
	display.drawText(0,15,"USB")
	display.drawText(50,0,str(vbatt)+"v")
	display.drawText(50,15,str(vusb)+"v")
	
	dataPoints.pop(0)
	dataPoints.append(round(vbatt*8))
	for i in range(64):
		display.drawPixel(i,display.height()-1-dataPoints[i],0xFFFFFF)
	
	dataPoints2.pop(0)
	dataPoints2.append(round(vusb*8))
	for i in range(64):
		display.drawPixel(64+i,display.height()-1-dataPoints2[i],0xFFFFFF)
		
	display.flush()

while True:
	vbatt = voltages.battery()/1000.0
	vusb = voltages.usb()/1000.0
	draw(vbatt, vusb)
	time.sleep(0.1)
