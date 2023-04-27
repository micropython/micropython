from CircuitOS import Input
from micropython import const
from .Nuvoton import Nuvoton


class WheelInput(Input):

    BYTE_NUM_EVENTS = const(0x40)
    BYTE_EVENTS = const(0x41)

    def __init__(self, nuvo: Nuvoton):
        super().__init__(6)
        self.nuvo = nuvo

    def get_num_events(self):
        data = self.nuvo.write_read(self.BYTE_NUM_EVENTS, 1)
        return data[0]

    def scan(self):
        num_events = self.get_num_events()
        if num_events == 0:
            return
        self.handle_events(num_events)

    def handle_events(self, num_events):
        data = self.nuvo.write_read([self.BYTE_EVENTS, num_events], num_events)
        for event in data:
            event_id = event & 0x7F
            state = event >> 7
            self.handle_single_event(event_id, state)

    def handle_single_event(self, event_id, state):
        if state:
            self.pressed(event_id)
        else:
            self.released(event_id)
