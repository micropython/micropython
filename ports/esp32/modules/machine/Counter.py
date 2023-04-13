# Counter.py

from machine import PCNT

class Counter(PCNT):
    IRQ_MATCH1 = PCNT.EVT_L_LIM
    IRQ_MATCH2 = PCNT.EVT_H_LIM
    IRQ_ZERO   = PCNT.EVT_ZERO

    UP = True
    DOWN = False

    INT16_ROLL = 15 # 32767

    def __init__(self, id, src=None, direction=UP, filter_ns=0):
        super().__init__(id)
        self.init(src=src, direction=direction, filter_ns=filter_ns)
        self.unit_id = id
        self.counter = 0
        self.match_handler = { }    
        self.event_enable(PCNT.EVT_L_LIM)
        self.event_enable(PCNT.EVT_H_LIM)
        self.in_handler = False
        super().irq(self.handler)

    def init(self, src=None, direction=UP, filter_ns=0):
        self.config(pin = src, channel = 0, 
            pos_mode = PCNT.COUNT_INC, neg_mode = PCNT.COUNT_DIS, 
            lctrl_mode = PCNT.MODE_KEEP if direction else PCNT.MODE_REVERSE,
            hctrl_mode = PCNT.MODE_REVERSE if direction else PCNT.MODE_KEEP,
            counter_l_lim = -Counter.INT16_ROLL, counter_h_lim = Counter.INT16_ROLL)
        self.filter(filter_ns)
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
        status = obj.event_status()

        if status & PCNT.EVT_L_LIM:
            self.counter -= Counter.INT16_ROLL

        if status & PCNT.EVT_H_LIM:
            self.counter += Counter.INT16_ROLL
        
        for e in [ PCNT.EVT_L_LIM, PCNT.EVT_H_LIM, PCNT.EVT_ZERO ]:
            if status & e and e in self.match_handler:
                self.in_handler = True
                self.match_handler[e](self)
                self.in_handler = False

        obj.event_status(True)   # ack event
        
    def irq(self, handler, event):
        if event:
            self.event_enable(event)
            self.match_handler[event] = handler
        else:
            self.event_disable(event)            

    def status(self):
        return self.event_status()
      
    def id(self):
        return self.unit_id

    def deinit(self):
        pass    
