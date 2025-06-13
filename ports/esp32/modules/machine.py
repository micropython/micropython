from umachine import *

import esp32

if hasattr(esp32, "PCNT"):

    class Counter:
        _INSTANCES = {}
        _PCNT = esp32.PCNT
        RISING = 1
        FALLING = 2
        UP = _PCNT.INCREMENT
        DOWN = _PCNT.DECREMENT

        def __new__(cls, unit_id, *args, **kwargs):
            self = cls._INSTANCES.get(unit_id)
            if self is None:
                self = super(Counter, cls).__new__(cls)
                self._pcnt = self._PCNT(minimum=-32000, maximum=32000)
                self._offset = 0
                cls._INSTANCES[unit_id] = self
            if args or kwargs:
                self.init(*args, **kwargs)
            return self

        def __init__(self, unit_id, *args, **kwargs):
            pass

        def init(self, *args, **kwargs):
            self._init(*args, **kwargs)
            self._pcnt.irq(self._wrap, self._PCNT.IRQ_MINIMUM | self._PCNT.IRQ_MAXIMUM)
            self._pcnt.start()

        def deinit(self):
            self._pcnt.deinit()
            self._pcnt.init(minimum=-32000, maximum=32000)
            self._offset = 0

        def value(self, value=None):
            # This loop deals with the possibility that a PCNT wrap occurs
            # between retrieving self._offset and self._pcnt.value()
            while True:
                offset = self._offset
                current = self._pcnt.value()
                if self._offset == offset:
                    break
            current += offset
            if value is not None:
                # In-place addition for atomicity
                self._offset += value - current
            return current

        def _wrap(self, mask):
            if mask & self._PCNT.IRQ_MINIMUM:
                self._offset -= 32000
            elif mask & self._PCNT.IRQ_MAXIMUM:
                self._offset += 32000

        def _init(self, src, edge=RISING, direction=UP, filter_ns=0):
            self._pcnt.init(
                pin=src,
                rising=direction if edge & self.RISING else self._PCNT.IGNORE,
                falling=direction if edge & self.FALLING else self._PCNT.IGNORE,
                filter=min(max(0, filter_ns * 80 // 1000), 1023),
            )

    class Encoder(Counter):
        _INSTANCES = {}

        def _init(self, phase_a, phase_b, filter_ns=0, phases=4):
            if phases not in (1, 2, 4):
                raise ValueError("phases")
            self._pcnt.init(
                pin=phase_a,
                falling=self._PCNT.INCREMENT,
                rising=self._PCNT.DECREMENT,
                mode_pin=phase_b,
                mode_low=self._PCNT.HOLD if phases == 1 else self._PCNT.REVERSE,
                filter=min(max(0, filter_ns * 80 // 1000), 1023),
            )
            if phases == 4:
                self._pcnt.init(
                    channel=1,
                    pin=phase_b,
                    falling=self._PCNT.DECREMENT,
                    rising=self._PCNT.INCREMENT,
                    mode_pin=phase_a,
                    mode_low=self._PCNT.REVERSE,
                )
            else:
                self._pcnt.init(channel=1, pin=None, rising=self._PCNT.IGNORE)
            self._phases = phases

        def phases(self):
            return self._phases


del esp32
