"""
    MikroElektronika Clicker 2
    Very Basic Board Support Package

    LTC3586-2 Power Supply and Charger
    ADC = 3V3, 12 bit, 0.806mV per bit

    Created by: Roland van Straten (last edit 20-06-2018). github: @rolandvs

    TODO:   Use calculated reference due to use of battery instead of fixed 3.3V value.
    TODO:   Add Interrupt to FAULT / CHARGE pins to indicate status as an event
            Read the ADC value three times and average the result or pick highest?!

    REMARK: Adding a switch or jumper to BOOT0 pin would be a great improvement of the board itself!

"""

from pyb import Switch
from pyb import Pin
from pyb import ADC
from pyb import delay


class BSP():
    """ Board Support Package of the CLICKER2 Board """

    def __init__(self):
        ''' init the board support package '''
        # battery measurement
        self.SenSel = Pin(Pin.cpu.B12, Pin.OUT_PP)
        self.SenSel.value(1)    # disable by default
        # analog
        self.VSense = ADC(Pin.cpu.C5)
        # status pins from LTC3586 (low active)
        self.Fault = Pin(Pin.cpu.C6, Pin.IN, Pin.PULL_NONE)
        self.BatStat = Pin(Pin.cpu.D4, Pin.IN, Pin.PULL_NONE)
        # first button, tied to PE0 and supported through pyb
        self.Button1 = Switch()
        # second button, not bound to micropython core
        self.Button2 = Pin(Pin.cpu.A10, Pin.IN, Pin.PULL_NONE)

    def ReadBatteryVoltage(self):
        ''' determine exact battery voltage  '''
        value = self.VSense.read()  # dummy read
        self.SenSel.value(0)        # activate measurement circuit
        delay(5)                    # some time to settle
        value = self.VSense.read()
        delay(5)
        value += self.VSense.read()
        delay(5)
        value += self.VSense.read()
        self.SenSel.value(1)        # deactive
        # calculate the "actual" voltage (voltage divider 1/3 (200k+100k))
        value = value * 3.3 / 4096
        return value

    def ReadChargerStatus(self):
        ''' read the status pins and report back the interpretation of them '''
        # can we give this more meaning?
        return self.IsFaultActive(), self.IsChargerActive()

    def IsChargerActive(self):
        ''' return True when charger is active '''
        return (self.BatStat.value() == 0)

    def IsFaultActive(self):
        ''' return True when fault is active '''
        return (self.Fault.value() == 0)

    def IsButton1Pressed(self):
        return self.Button1()

    def IsButton2Pressed(self):
        return (self.Button2.value() == 0)


if __name__ == '__main__':
    ''' the program that exercises the different functions when loaded through
        execfile('')
    '''
    # create bsp object
    clicker2 = BSP()
    print("Loaded CLICKER2 Board Support Package")
    print("Battery Voltage is {:4.2f}".format(clicker2.ReadBatteryVoltage()))
    if clicker2.IsChargerActive():
        print("Battery is charging")
    if clicker2.IsFaultActive():
        print("Oops there is a problem with the LTC3586")
