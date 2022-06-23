# Encoder.py

# Usage:
# from machine import Pin, Encoder
#
# enc = Encoder(0, Pin(18), Pin(19))
# print("Encoder:", enc.value())

from machine import PCNT

class Encoder(PCNT):
    LIMIT =  100

    def __init__(self, unit_id, phase_a, phase_b):
        super().__init__(unit_id)
        self.unit_id = unit_id

        # react on pin_a edges
        self.config(pin = phase_a, ctrl = phase_b, channel = 0, 
            pos_mode = PCNT.COUNT_DEC, neg_mode = PCNT.COUNT_INC, 
            lctrl_mode = PCNT.MODE_REVERSE, hctrl_mode = PCNT.MODE_KEEP,
            counter_l_lim = -self.LIMIT, counter_h_lim = self.LIMIT)

        # react on pin_b edges
        self.config(pin = phase_b, ctrl = phase_a, channel = 1, 
            pos_mode = PCNT.COUNT_INC, neg_mode = PCNT.COUNT_DEC, 
            lctrl_mode = PCNT.MODE_REVERSE, hctrl_mode = PCNT.MODE_KEEP,
            counter_l_lim = -self.LIMIT, counter_h_lim = self.LIMIT)

        self.event_enable(PCNT.EVT_H_LIM);
        self.event_enable(PCNT.EVT_L_LIM);
        super().irq(self.handler)

        self.counter = 0
        self.counter_clear()

    def pause(self):
        self.counter_pause()

    def resume(self):
        self.counter_resume()

    def filter(self, *argv):
        # convert from nanoseconds into PCNT units
        if len(argv) > 0:
            self.filter_value(2*argv[0]//25)
        else:
            return 25*self.filter_value()//2

    def value(self, *argv):
        # set/get the counter value

        # read value. Make sure no events are pending
        count = None
        while count == None:
            count = self.counter_value(True)

        counter = self.counter

        if len(argv) > 0:
            if argv[0]:
                self.counter = argv[0] - count
            else:
                self.counter_clear()
                self.counter = 0

        return counter + count

    def handler(self, obj):
        if obj.event_status() & PCNT.EVT_L_LIM:
            self.counter -= self.LIMIT

        if obj.event_status() & PCNT.EVT_H_LIM:
            self.counter += self.LIMIT

        obj.event_status(True)   # ack event

    def deinit(self):
        pass    
