import display, hardware, mascot

# Device specific system functions

def configureWakeupSource():
    return False # Not supported

def prepareForSleep():
    pass # Not supported

def prepareForWakeup():
    showLoadingScreen()
    hardware.enable_power()
    try:
        hardware.mountsd()
    except:
        pass

def showLoadingScreen(app="BadgePython"):
    display.drawFill(0xFFFFFF)
    snek_info = display.pngInfo(mascot.snek)
    display.drawPng((display.width() - snek_info[0]) // 2, (display.height() - snek_info[1]) // 2 - (snek_info[1] // 2) - 10, mascot.snek)
    text = "Starting{}...".format("" if app == "" else " " + app)
    display.drawText((display.width() - display.getTextWidth(text)) // 2, (display.height() - snek_info[1]) // 2 + (snek_info[1] // 2) + 10, text, 0x000000)
    display.flush()

def showMessage(message="", icon=None):
    display.drawFill(0x0000FF)
    display.drawText(10, 10, message, 0xFFFFFF)
    display.flush()
