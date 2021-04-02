# rp2 module: uses C code from _rp2, plus asm_pio decorator implemented in Python.
# MIT license; Copyright (c) 2020-2021 Damien P. George

from _rp2 import *
from micropython import const

_PROG_DATA = const(0)
_PROG_OFFSET_PIO0 = const(1)
_PROG_OFFSET_PIO1 = const(2)
_PROG_EXECCTRL = const(3)
_PROG_SHIFTCTRL = const(4)
_PROG_OUT_PINS = const(5)
_PROG_SET_PINS = const(6)
_PROG_SIDESET_PINS = const(7)
_PROG_MAX_FIELDS = const(8)


class PIOASMError(Exception):
    pass


class PIOASMEmit:
    def __init__(
        self,
        *,
        out_init=None,
        set_init=None,
        sideset_init=None,
        in_shiftdir=0,
        out_shiftdir=0,
        autopush=False,
        autopull=False,
        push_thresh=32,
        pull_thresh=32,
        fifo_join=0
    ):
        # uarray is a built-in module so importing it here won't require
        # scanning the filesystem.
        from uarray import array

        self.labels = {}
        execctrl = 0
        shiftctrl = (
            fifo_join << 30
            | (pull_thresh & 0x1F) << 25
            | (push_thresh & 0x1F) << 20
            | out_shiftdir << 19
            | in_shiftdir << 18
            | autopull << 17
            | autopush << 16
        )
        self.prog = [array("H"), -1, -1, execctrl, shiftctrl, out_init, set_init, sideset_init]

        self.wrap_used = False

        if sideset_init is None:
            self.sideset_count = 0
        elif isinstance(sideset_init, int):
            self.sideset_count = 1
        else:
            self.sideset_count = len(sideset_init)

    def start_pass(self, pass_):
        if pass_ == 1:
            if not self.wrap_used and self.num_instr:
                self.wrap()
            self.delay_max = 31
            if self.sideset_count:
                self.sideset_opt = self.num_sideset != self.num_instr
                if self.sideset_opt:
                    self.prog[_PROG_EXECCTRL] |= 1 << 30
                    self.sideset_count += 1
                self.delay_max >>= self.sideset_count
        self.pass_ = pass_
        self.num_instr = 0
        self.num_sideset = 0

    def __getitem__(self, key):
        return self.delay(key)

    def delay(self, delay):
        if self.pass_ > 0:
            if delay > self.delay_max:
                raise PIOASMError("delay too large")
            self.prog[_PROG_DATA][-1] |= delay << 8
        return self

    def side(self, value):
        self.num_sideset += 1
        if self.pass_ > 0:
            set_bit = 13 - self.sideset_count
            self.prog[_PROG_DATA][-1] |= self.sideset_opt << 12 | value << set_bit
        return self

    def wrap_target(self):
        self.prog[_PROG_EXECCTRL] |= self.num_instr << 7

    def wrap(self):
        assert self.num_instr
        self.prog[_PROG_EXECCTRL] |= (self.num_instr - 1) << 12
        self.wrap_used = True

    def label(self, label):
        if self.pass_ == 0:
            if label in self.labels:
                raise PIOASMError("duplicate label {}".format(label))
            self.labels[label] = self.num_instr

    def word(self, instr, label=None):
        self.num_instr += 1
        if self.pass_ > 0:
            if label is None:
                label = 0
            else:
                if not label in self.labels:
                    raise PIOASMError("unknown label {}".format(label))
                label = self.labels[label]
            self.prog[_PROG_DATA].append(instr | label)
        return self

    def nop(self):
        return self.word(0xA042)

    def jmp(self, cond, label=None):
        if label is None:
            label = cond
            cond = 0  # always
        return self.word(0x0000 | cond << 5, label)

    def wait(self, polarity, src, index):
        if src == 6:
            src = 1  # "pin"
        elif src != 0:
            src = 2  # "irq"
        return self.word(0x2000 | polarity << 7 | src << 5 | index)

    def in_(self, src, data):
        if not 0 < data <= 32:
            raise PIOASMError("invalid bit count {}".format(data))
        return self.word(0x4000 | src << 5 | data & 0x1F)

    def out(self, dest, data):
        if dest == 8:
            dest = 7  # exec
        if not 0 < data <= 32:
            raise PIOASMError("invalid bit count {}".format(data))
        return self.word(0x6000 | dest << 5 | data & 0x1F)

    def push(self, value=0, value2=0):
        value |= value2
        if not value & 1:
            value |= 0x20  # block by default
        return self.word(0x8000 | (value & 0x60))

    def pull(self, value=0, value2=0):
        value |= value2
        if not value & 1:
            value |= 0x20  # block by default
        return self.word(0x8080 | (value & 0x60))

    def mov(self, dest, src):
        if dest == 8:
            dest = 4  # exec
        return self.word(0xA000 | dest << 5 | src)

    def irq(self, mod, index=None):
        if index is None:
            index = mod
            mod = 0  # no modifiers
        return self.word(0xC000 | (mod & 0x60) | index)

    def set(self, dest, data):
        return self.word(0xE000 | dest << 5 | data)


_pio_funcs = {
    # source constants for wait
    "gpio": 0,
    # "pin": see below, translated to 1
    # "irq": see below function, translated to 2
    # source/dest constants for in_, out, mov, set
    "pins": 0,
    "x": 1,
    "y": 2,
    "null": 3,
    "pindirs": 4,
    "pc": 5,
    "status": 5,
    "isr": 6,
    "osr": 7,
    "exec": 8,  # translated to 4 for mov, 7 for out
    # operation functions for mov's src
    "invert": lambda x: x | 0x08,
    "reverse": lambda x: x | 0x10,
    # jmp condition constants
    "not_x": 1,
    "x_dec": 2,
    "not_y": 3,
    "y_dec": 4,
    "x_not_y": 5,
    "pin": 6,
    "not_osre": 7,
    # constants for push, pull
    "noblock": 0x01,
    "block": 0x21,
    "iffull": 0x40,
    "ifempty": 0x40,
    # constants and modifiers for irq
    # "noblock": see above
    # "block": see above
    "clear": 0x40,
    "rel": lambda x: x | 0x10,
    # functions
    "wrap_target": None,
    "wrap": None,
    "label": None,
    "word": None,
    "nop": None,
    "jmp": None,
    "wait": None,
    "in_": None,
    "out": None,
    "push": None,
    "pull": None,
    "mov": None,
    "irq": None,
    "set": None,
}


def asm_pio(**kw):
    emit = PIOASMEmit(**kw)

    def dec(f):
        nonlocal emit

        gl = _pio_funcs
        gl["wrap_target"] = emit.wrap_target
        gl["wrap"] = emit.wrap
        gl["label"] = emit.label
        gl["word"] = emit.word
        gl["nop"] = emit.nop
        gl["jmp"] = emit.jmp
        gl["wait"] = emit.wait
        gl["in_"] = emit.in_
        gl["out"] = emit.out
        gl["push"] = emit.push
        gl["pull"] = emit.pull
        gl["mov"] = emit.mov
        gl["irq"] = emit.irq
        gl["set"] = emit.set

        old_gl = f.__globals__.copy()
        f.__globals__.clear()
        f.__globals__.update(gl)

        emit.start_pass(0)
        f()

        emit.start_pass(1)
        f()

        f.__globals__.clear()
        f.__globals__.update(old_gl)

        return emit.prog

    return dec


# sideset_count is inclusive of enable bit
def asm_pio_encode(instr, sideset_count):
    emit = PIOASMEmit()
    emit.delay_max = 31
    emit.sideset_count = sideset_count
    if emit.sideset_count:
        emit.delay_max >>= emit.sideset_count
    emit.pass_ = 1
    emit.num_instr = 0
    emit.num_sideset = 0

    gl = _pio_funcs
    gl["nop"] = emit.nop
    # gl["jmp"] = emit.jmp currently not supported
    gl["wait"] = emit.wait
    gl["in_"] = emit.in_
    gl["out"] = emit.out
    gl["push"] = emit.push
    gl["pull"] = emit.pull
    gl["mov"] = emit.mov
    gl["irq"] = emit.irq
    gl["set"] = emit.set

    exec(instr, gl)

    if len(emit.prog[_PROG_DATA]) != 1:
        raise PIOASMError("expecting exactly 1 instruction")
    return emit.prog[_PROG_DATA][0]
