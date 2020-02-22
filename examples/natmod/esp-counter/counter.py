import struct, espidf

# Directions (PCNT_COUNT_* in components/driver/include/driver/pcnt.h)
UP=1
DOWN=2

# Edges
RISING=1
FALLING=2

# Private constants
_COUNT_DIS = 0
_COUNT_INC = 1
_COUNT_DEC = 2
_MODE_KEEP = 0
_MODE_REVERSE = 1
_MODE_DISABLE = 2

def _err_raise(err):
    if err == 0: return
    raise ValueError(espidf.esp_err_to_name(err))

class Counter:
    def __init__(self, unit, pin, direction=UP, edge=RISING, limit=0, reset=True):
        # check unit number
        if unit < 0 or unit >= 8:
            raise ValueError("unit must be in range 0..7")
        self.unit = unit
        # init obj
        self.pin = pin
        # init unit
        self.init(direction, edge, limit, reset)

    def init(self, direction=UP, edge=RISING, limit=0, reset=True):
        # init config elements
        channel = 0
        pulse_gpio_num = self.pin
        ctrl_gpio_num = -1 # not yet supported
        counter_h_lim = 0
        counter_l_lim = 0
        pos_mode = _COUNT_DIS
        neg_mode = _COUNT_DIS
        lctrl_mode = _MODE_KEEP
        hctrl_mode = _MODE_KEEP
        #
        self.dir_up = direction == UP
        self.edges = edge
        if self.dir_up: mode = _COUNT_INC
        else:           mode = _COUNT_DEC
        # tweak config according to parameters
        if edge & RISING: pos_mode = mode
        if edge & FALLING: neg_mode = mode
        self.limit = limit
        if self.limit == 0: limit = 0x7ffff
        if self.dir_up:
            counter_h_lim = self.limit
        else:
            counter_l_lim = -self.limit
        # convert pcnt_config to C struct
        print('IIIIIIhhII', pulse_gpio_num, ctrl_gpio_num,
                lctrl_mode, hctrl_mode, pos_mode, neg_mode, counter_h_lim, counter_l_lim,
                self.unit, channel)
        pcnt_config_struct = bytearray(struct.calcsize('IIIIIIhhII'))
        struct.pack_into('IIIIIIhhII', pcnt_config_struct, 0,
                pulse_gpio_num, ctrl_gpio_num, lctrl_mode, hctrl_mode, pos_mode, neg_mode,
                counter_h_lim, counter_l_lim, self.unit, channel)
        _err_raise(espidf.pcnt_unit_config(pcnt_config_struct))
        _err_raise(espidf.pcnt_counter_resume(self.unit)) # apparently it starts paused...
        if reset:
            _err_raise(espidf.pcnt_counter_clear(self.unit))

    def __del__(self):
        _err_raise(espidf.pcnt_counter_pause(self.unit))
        _err_raise(espidf.pcnt_intr_disable(self.unit))
        _err_raise(espidf.pcnt_set_pin(self.unit, self.channel, -1, -1))
        self.unit = -1
        self.pin = -1

    def resume(self):
        _err_raise(espidf.pcnt_unit_resume(self.unit))

    def pause(self):
        _err_raise(espidf.pcnt_unit_pause(self.unit))

    def value(self, new_value=None):
        if new_value == None:
            # GET value
            v_raw = bytearray(2)
            _err_raise(espidf.pcnt_get_counter_value(self.unit, v_raw))
            v = struct.unpack('h', v_raw)
            if not self.dir_up:
                v[0] += self.limit # adjust to limit..0 interval
            return v[0]
        else:
            # SET value
            if self.dir_up and new_value != 0:
                raise ValueError("only v=0 supported")
            if not self.dir_up and new_value != self.limit:
                raise ValueError("only v=limit supported")
            _err_raise(espidf.pcnt_counter_clear(self.unit))
            return None
