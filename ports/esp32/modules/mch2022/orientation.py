import consts, valuestore, display, buttons

def getDefault():
    value = valuestore.load('system', 'orientation')
    if value != None:
        value = int(value)
    return value if value != None else consts.DEFAULT_ORIENTATION

def setDefault(value = consts.DEFAULT_ORIENTATION):
    if value == 0 or value == 90 or value == 180 or value == 270:
        valuestore.save('system', 'orientation', value)
        return True
    return False

#---

def default(rotateButtons=True):
    value = valuestore.load('system', 'orientation') 
    if value == None:
        value = consts.DEFAULT_ORIENTATION
    value = int(value)
    if rotateButtons:
        buttons.rotate(value)
    display.orientation(value)

def landscape(rotateButtons=True):
    if rotateButtons:
        buttons.rotate(0)
    display.orientation(0)

def portrait(rotateButtons=True):
    if rotateButtons:
        buttons.rotate(90)
    display.orientation(90)

def isLandscape(value=None):
    if value == None:
        value = display.orientation()
    if value==90 or value==270:
        return False
    return True

def isPortrait(value=getDefault()):
    if value==90 or value==270:
        return True
    return False
