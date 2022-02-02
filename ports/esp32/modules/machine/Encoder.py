# Encoder.py

from machine import PCNT, Pin

class Encoder(PCNT):
    IRQ_MATCH1 = PCNT.EVT_L_LIM
    IRQ_MATCH2 = PCNT.EVT_H_LIM

    def __init__(self, unit_id, phase_a, phase_b, match1, match2):
        super().__init__(unit_id)
        self.unit_id = unit_id

        # react on pin_a edges
        self.config(pin = phase_a, ctrl = phase_b, channel = 0, 
            pos_mode = PCNT.COUNT_DEC, neg_mode = PCNT.COUNT_INC, 
            lctrl_mode = PCNT.MODE_REVERSE, hctrl_mode = PCNT.MODE_KEEP,
            counter_l_lim = match1, counter_h_lim = match2)

        # react on pin_b edges
        self.config(pin = phase_b, ctrl = phase_a, channel = 1, 
            pos_mode = PCNT.COUNT_INC, neg_mode = PCNT.COUNT_DEC, 
            lctrl_mode = PCNT.MODE_REVERSE, hctrl_mode = PCNT.MODE_KEEP,
            counter_l_lim = match1, counter_h_lim = match2)

        self.counter = 0
        self.match_handler = { }
        super().irq(self.handler)

    def id(self):
        return self.unit_id

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
        counter = self.counter
        count = self.counter_value()

        if len(argv) > 0:
            if argv[0]:
                self.counter = argv[0] - count
            else:
                self.counter_clear()
                self.counter = 0

        return counter + count

    def handler(self, obj):
        status = obj.event_status()
        for e in [ PCNT.EVT_L_LIM, PCNT.EVT_H_LIM ]:
            if status & e and e in self.match_handler:
                self.match_handler[e](self)

    def irq(self, handler, event):
        self.event_enable(event)
        self.match_handler[event] = handler

    def deinit(self):
        pass    
