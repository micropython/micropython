import badgebsp


BTN_HOME=0x3B
BTN_MENU=0x3C
BTN_START=0x3D
BTN_SELECT=0x3E
BTN_ACCEPT=0x3F
BTN_A=BTN_ACCEPT
BTN_BACK=0x40
BTN_B=BTN_BACK
BTN_LEFT=0xe04b
BTN_PRESS=0x1c
BTN_DOWN=0xe050
BTN_UP=0xe048
BTN_RIGHT=0xe04d

_callbacks = {}

def _cb(scancode):
    global _callbacks
    scancode1 = scancode & ~0x80
    state = False if (scancode & 0x80) else True
    callback = _callbacks.get(scancode1)
    if callable(callback):
        callback(state)
    
def attach(scancode, action):
    global _callbacks
    _callbacks[scancode] = action
    return True
    
def detach(gpio):
    return attach(gpio, None)

def detachAll():
    global _callbacks
    _callbacks.clear()

def getCallback(scancode):
    callback = _callbacks.get(scancode)
    return callback

badgebsp.set_handler(_cb)
