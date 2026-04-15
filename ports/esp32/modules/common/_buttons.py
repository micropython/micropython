# Generic GPIO input wrapper
# Versions for other badges must expose the same API

import machine

_gpios     = []
_pins      = []
_callbacks = []
_states    = []

def _cb(pin):
	global _pins, _gpios, _callbacks, _states
	position = _pins.index(pin)
	gpio = _gpios[position]
	callback = _callbacks[position]
	prevState = _states[position]
	if callable(callback) and (prevState != pin.value()):
		_states[position] = pin.value()
		callback(not pin.value())
	
def register(gpio, action=None):
	global _pins, _gpios, _callbacks, _states
	if gpio in _gpios:
		return False
	pin = machine.Pin(gpio, machine.Pin.IN)
	pin.irq(handler=_cb, trigger=machine.Pin.IRQ_FALLING|machine.Pin.IRQ_RISING)
	_gpios.append(gpio)
	_pins.append(pin)
	_callbacks.append(action)
	_states.append(pin.value())
	return True

def attach(gpio, action):
	global _pins, _gpios, _callbacks
	if not gpio in _gpios:
		return False
	position = _gpios.index(gpio)
	_callbacks[position] = action
	return True
	
def detach(gpio):
	return attach(gpio, None)

def detachAll():
	for gpio in _gpios:
		attach(gpio, None)

def pin(gpio):
	global _pins, _gpios, _callbacks
	if not gpio in _gpios:
		return None
	position = _gpios.index(gpio)
	return _pins[position]

def value(gpio):
	pin = pin(gpio)
	if not pin:
		return None
	return pin.value()

def getCallback(gpio):
	global _pins, _gpios, _callbacks
	if not gpio in _gpios:
		return None
	position = _pins.index(pin)
	callback = _callbacks[position]
	return callback
