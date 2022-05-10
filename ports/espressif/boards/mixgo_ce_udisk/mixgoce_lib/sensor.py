"""
Analog Sensor

CircuitPython library for Analog Sensor - MixGoCE
=======================================================

Small Cabbage
20210721
dahanzimin
20210423
"""
from mixgoce import version

class ADCSensor:
    import board
    __pins=[board.IO13,board.IO15,board.IO16]
    __species = {}
    __first_init = True
    def __new__(cls, pin, *args, **kwargs):
        if pin not in cls.__species.keys():
            cls.__first_init = True
            cls.__species[pin]=object.__new__(cls)
        return cls.__species[pin]

    def __init__(self,pin):
        from analogio import AnalogIn
        if self.__first_init:
            self.__first_init = False
            self.pin = AnalogIn(self.__pins[pin])

    def read(self):
        return self.pin.value

def get_brightness():
    return ADCSensor(1).read() 

def get_soundlevel():  #fix
	value_d= []
	for _ in range(5):
		values = []
		for _ in range(5):
			val = ADCSensor(0).read() 
			values.append(val)
		value_d.append(max(values) - min(values))
	return  max(value_d)


if version:#new
    from mixgoce import acc
    def get_temperature():
        return acc.get_temperature()
else:#old
    def get_temperature():
        adc_val = ADCSensor(2).read() 
        return adc_val * 3.9 / 5900