# Generic button input wrapper
from hardware.buttonconsts import *
import hardware.buttons as hwButtons
import system

# Check if all arrow keys are definied to enable rotation support
if "BTN_LEFT" in globals() and "BTN_RIGHT" in globals() and "BTN_UP" in globals() and "BTN_DOWN" in globals():
    __arrows = [BTN_DOWN, BTN_LEFT, BTN_UP, BTN_RIGHT]
else:
    __arrows = []

# Internal variables
__orientation = 0
__cb = []

# Internal functions
def cbReboot(pressed):
    if pressed:
        system.launcher()

def cbHome(pressed):
    if pressed:
        system.home()

def cbMenu(pressed):
    if pressed:
        system.launcher()

def __cb_btn(btn, state):
    if btn in __arrows:
        index = __arrows.index(btn)
        index = (index + __orientation // 90) % 4
        btn = __arrows[index]
    if btn not in __cb[-1]:
        print("Missing button mapping")
    elif __cb[-1][btn]:
        __cb[-1][btn](state)
        

def __init():
    hwButtons.register_callback(__cb_btn)
    pushMapping() # Add the initial / default mapping
    
# Public functions

def attach(button, callback):
    '''
    Attach a callback function to a button
    '''
    global __cb
    if button not in ALL_BUTTONS:
        raise ValueError("Invalid button!")
    __cb[-1][button] = callback

def detach(button):
    '''
    Detach the callback function from a button
    '''
    global __cb
    if button not in ALL_BUTTONS:
        raise ValueError("Invalid button!")
    __cb[-1][button] = None

def value(button):
    '''
    Read the state of a button (polling)
    '''
    global _buttons
    if button not in ALL_BUTTONS:
        raise ValueError("Invalid button!")
    return hwButtons.value(button)

def getCallback(button):
    '''
    Returns the currently attached callback function
    '''
    global __cb
    if button not in ALL_BUTTONS:
        raise ValueError("Invalid button!")
    return __cb[-1][button]

def pushMapping(newMapping=None):
    '''
    Push a new button mapping on the stack
    '''
    global __cb
    if newMapping == None:
        newMapping = DEFAULT_MAPPING
    __cb.append(newMapping)

def popMapping():
    '''
    Pop the current button mapping from the stack
    '''
    global __cb
    if len(__cb) > 0:
        __cb = __cb[:-1]
    if len(__cb) < 1:
        pushMapping()

def rotate(value):
    '''
    Change the orientation of the arrow keys (0, 90, 180, 270)
    '''
    global __orientation
    __orientation = value

# ---
__init()
