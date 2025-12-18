import sys

_path = sys.path
sys.path = ()
try:
    import machine as _machine
finally:
    sys.path = _path
    del _path
    del sys


from micropython import const
import esp32

if hasattr(esp32, "PCNT"):
    _PCNT_RANGE = const(32000)

    class _CounterBase:
        _PCNT = esp32.PCNT
        # Singletons, keyed by PCNT unit_id (shared by both Counter & Encoder).
        _INSTANCES = {}

        # Use __new__ to implement a singleton rather than a factory function,
        # because we need to be able to provide class attributes, e.g.
        # Counter.RISING, which is not possible if Counter was a function
        # (functions cannot have attributes in MicroPython).
        def __new__(cls, unit_id, *_args, **_kwargs):
            # Find an existing instance for this PCNT unit id.
            self = cls._INSTANCES.get(unit_id)

            if self:
                # Verify that this PCNT is being used for the same type
                # (Encoder or Counter).
                if not isinstance(self, cls):
                    raise ValueError("PCNT in use")
            else:
                # Previously unused PCNT unit.
                self = object.__new__(cls)
                cls._INSTANCES[unit_id] = self

            # __init__ will now be called with the same args.
            return self

        def __init__(self, unit_id, *args, filter_ns=0, **kwargs):
            self._unit_id = unit_id

            if not hasattr(self, "_pcnt"):
                # New instance, or previously deinit-ed.
                self._pcnt = self._PCNT(unit_id, min=-_PCNT_RANGE, max=_PCNT_RANGE)
            elif not (args or kwargs):
                # Existing instance, and no args, so accessing the existing
                # singleton without reconfiguring. Note: This means that
                # Counter/Encoder cannot be partially re-initalised. Either
                # you get the existing instance as-is (by passing no arguments
                # other than the id), or you must pass all the necessary
                # arguments to additionally re-configure it.
                return

            # Counter- or Encoder-specific configuration of self._pcnt.
            self._configure(*args, **kwargs)

            # Common unit configuration.
            self._pcnt.init(
                filter=min(max(0, filter_ns * 80 // 1000), 1023),
                value=0,
            )

            # Note: We track number-of-overflows rather than the actual count in
            # order to avoid the IRQ handler overflowing MicroPython's "small int"
            # range. This gives an effective range of 2**30 overflows. User code
            # should use counter.value(0) to reset the overflow count.
            # The ESP32 PCNT resets to zero on under/overflow (i.e. it does not wrap
            # around to the opposite limit), so each overflow corresponds to exactly
            # _PCNT_RANGE counts.

            # Reset counter state.
            self._overflows = 0
            self._offset = 0

            # Install IRQ handler to handle under/overflow.
            self._pcnt.irq(self._overflow, self._PCNT.IRQ_MIN | self._PCNT.IRQ_MAX)

            # Start counting.
            self._pcnt.start()

        # Handle counter under/overflow.
        def _overflow(self, pcnt):
            mask = pcnt.irq().flags()
            if mask & self._PCNT.IRQ_MIN:
                self._overflows -= 1
            elif mask & self._PCNT.IRQ_MAX:
                self._overflows += 1

        # Public machine.Counter & machine.Encoder API.
        def init(self, *args, **kwargs):
            self.__init__(self._unit_id, *args, **kwargs)

        # Public machine.Counter & machine.Encoder API.
        def deinit(self):
            if hasattr(self, "_pcnt"):
                self._pcnt.deinit()
                del self._pcnt

        # Public machine.Counter & machine.Encoder API.
        def value(self, value=None):
            if not hasattr(self, "_pcnt"):
                raise RuntimeError("not initialised")

            # This loop deals with the possibility that a PCNT overflow occurs
            # between retrieving self._overflows and self._pcnt.value().
            while True:
                overflows = self._overflows
                current = self._pcnt.value()
                # Calling PCNT.value() forces any pending interrupts to run
                # for this PCNT unit. So self._overflows must now be the the
                # value corresponding to the value we read.
                if self._overflows == overflows:
                    break

            # Compute the result including the number of times we've cycled
            # through the range, and any applied offset.
            result = overflows * _PCNT_RANGE + current + self._offset

            # If a new value is specified, then zero out the overflows, and set
            # self._offset so that it zeros out the current PCNT value. The
            # mutation to self._overflows is atomic w.r.t. the overflow IRQ
            # handler because the scheduler only runs on branch instructions.
            if value is not None:
                self._overflows -= overflows
                self._offset = value - current

            return result

    class Counter(_CounterBase):
        # Public machine.Counter API.
        RISING = 1
        FALLING = 2
        UP = _CounterBase._PCNT.INCREMENT
        DOWN = _CounterBase._PCNT.DECREMENT

        # Counter-specific configuration.
        def _configure(self, src, edge=RISING, direction=UP):
            # Only use the first channel.
            self._pcnt.init(
                channel=0,
                pin=src,
                rising=direction if edge & Counter.RISING else self._PCNT.IGNORE,
                falling=direction if edge & Counter.FALLING else self._PCNT.IGNORE,
            )

    class Encoder(_CounterBase):
        # Encoder-specific configuration.
        def _configure(self, phase_a, phase_b, phases=1):
            if phases not in (1, 2, 4):
                raise ValueError("phases")
            # Configure the first channel.
            self._pcnt.init(
                channel=0,
                pin=phase_a,
                falling=self._PCNT.INCREMENT,
                rising=self._PCNT.DECREMENT,
                mode_pin=phase_b,
                mode_low=self._PCNT.HOLD if phases == 1 else self._PCNT.REVERSE,
            )
            if phases == 4:
                # For 4x quadrature, enable the second channel.
                self._pcnt.init(
                    channel=1,
                    pin=phase_b,
                    falling=self._PCNT.DECREMENT,
                    rising=self._PCNT.INCREMENT,
                    mode_pin=phase_a,
                    mode_low=self._PCNT.REVERSE,
                )
            else:
                # For 1x and 2x quadrature, disable the second channel.
                self._pcnt.init(channel=1, pin=None, rising=self._PCNT.IGNORE)
            self._phases = phases

        def phases(self):
            return self._phases

    del _CounterBase


del esp32


# Delegate to built-in machine module.
def __getattr__(attr):
    return getattr(_machine, attr)
