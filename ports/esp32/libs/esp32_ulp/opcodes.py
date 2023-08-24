"""
ESP32 ULP Co-Processor Instructions
"""

from ucollections import namedtuple
from uctypes import struct, addressof, LITTLE_ENDIAN, UINT32, BFUINT32, BF_POS, BF_LEN

from .soc import *
from .util import split_tokens, validate_expression

# XXX dirty hack: use a global for the symbol table
symbols = None

# Opcodes, Sub-Opcodes, Modes, ...

OPCODE_WR_REG = 1
OPCODE_RD_REG = 2

DR_REG_MAX_DIRECT = 0x3ff
RD_REG_PERIPH_RTC_CNTL = 0
RD_REG_PERIPH_RTC_IO = 1
RD_REG_PERIPH_SENS = 2
RD_REG_PERIPH_RTC_I2C = 3

OPCODE_I2C = 3

OPCODE_DELAY = 4

OPCODE_ADC = 5

OPCODE_ST = 6
SUB_OPCODE_ST = 4

OPCODE_ALU = 7
SUB_OPCODE_ALU_REG = 0
SUB_OPCODE_ALU_IMM = 1
ALU_SEL_ADD = 0
ALU_SEL_SUB = 1
ALU_SEL_AND = 2
ALU_SEL_OR = 3
ALU_SEL_MOV = 4
ALU_SEL_LSH = 5
ALU_SEL_RSH = 6
SUB_OPCODE_ALU_CNT = 2
ALU_SEL_INC = 0
ALU_SEL_DEC = 1
ALU_SEL_RST = 2

OPCODE_BRANCH = 8
# https://github.com/espressif/binutils-esp32ulp/blob/d61f86f97eda43fc118df30d019fc062aaa6bc8d/include/opcode/esp32ulp_esp32.h#L85
SUB_OPCODE_BX = 0
SUB_OPCODE_BR = 1
SUB_OPCODE_BS = 2
BX_JUMP_TYPE_DIRECT = 0
BX_JUMP_TYPE_ZERO = 1
BX_JUMP_TYPE_OVF = 2
# https://github.com/espressif/binutils-esp32ulp/blob/d61f86f97eda43fc118df30d019fc062aaa6bc8d/gas/config/tc-esp32ulp.h#L91
BRCOND_LT = 0
BRCOND_GE = 1
BRCOND_LE = 2
BRCOND_EQ = 3
BRCOND_GT = 4

OPCODE_END = 9
SUB_OPCODE_END = 0
SUB_OPCODE_SLEEP = 1

OPCODE_TSENS = 10

OPCODE_HALT = 11

OPCODE_LD = 13


def make_ins_struct_def(layout):
    lines = layout.strip().splitlines()
    pos = 0  # bitfield definitions start from lsb
    struct_def = {}
    for line in lines:
        bitfield = line.split('#', 1)[0]  # get rid of comment
        name, width = bitfield.split(':', 1)
        name = name.strip()
        width = int(width.strip())
        struct_def[name] = BFUINT32 | pos << BF_POS | width << BF_LEN
        pos += width
    if pos != 32:
        raise ValueError('make_ins: bit field widths must sum up to 32. [%s]' % layout)
    struct_def['all'] = UINT32
    return struct_def


def make_ins(layout):
    """
    transform textual instruction layout description into a ready-to-use uctypes struct
    """
    struct_def = make_ins_struct_def(layout)
    instruction = bytearray(4)
    return struct(addressof(instruction), struct_def, LITTLE_ENDIAN)


# instruction structure definitions

_wr_reg = make_ins("""
    addr : 8        # Address within either RTC_CNTL, RTC_IO, or SARADC
    periph_sel : 2  # Select peripheral: RTC_CNTL (0), RTC_IO(1), SARADC(2)
    data : 8        # 8 bits of data to write
    low : 5         # Low bit
    high : 5        # High bit
    opcode : 4      # Opcode (OPCODE_WR_REG)
""")


_rd_reg = make_ins("""
    addr : 8        # Address within either RTC_CNTL, RTC_IO, or SARADC
    periph_sel : 2  # Select peripheral: RTC_CNTL (0), RTC_IO(1), SARADC(2)
    unused : 8      # Unused
    low : 5         # Low bit
    high : 5        # High bit
    opcode : 4      # Opcode (OPCODE_RD_REG)
""")


_i2c = make_ins("""
    sub_addr : 8    # address within I2C slave
    data : 8        # Data to write (not used for read)
    low : 3         # low bit
    high : 3        # high bit
    i2c_sel : 4     # select i2c slave via SENS_I2C_SLAVE_ADDRx
    unused : 1      # Unused
    rw : 1          # Write (1) or read (0)
    opcode : 4      # Opcode (OPCODE_I2C)
""")


_delay = make_ins("""
    cycles : 16     # Number of cycles to sleep
    unused : 12     # Unused
    opcode : 4      # Opcode (OPCODE_DELAY)
""")


_tsens = make_ins("""
    dreg : 2        # Register where to store TSENS result
    delay : 14      # Number of cycles needed to obtain a measurement
    unused : 12     # Unused
    opcode : 4      # Opcode (OPCODE_TSENS)
""")


_adc = make_ins("""
    dreg : 2        # Register where to store ADC result
    mux : 4         # Select SARADC pad (mux + 1)
    sar_sel : 1     # Select SARADC0 (0) or SARADC1 (1)
    unused1 : 1     # Unused
    cycles : 16     # TBD, cycles used for measurement
    unused2 : 4     # Unused
    opcode: 4       # Opcode (OPCODE_ADC)
""")


_st = make_ins("""
    sreg : 2        # Register which contains data to store
    dreg : 2        # Register which contains address in RTC memory (expressed in words)
    unused1 : 6     # Unused
    offset : 11     # Offset to add to dreg
    unused2 : 4     # Unused
    sub_opcode : 3  # Sub opcode (SUB_OPCODE_ST)
    opcode : 4      # Opcode (OPCODE_ST)
""")


_alu_reg = make_ins("""
    dreg : 2        # Destination register
    sreg : 2        # Register with operand A
    treg : 2        # Register with operand B
    unused : 15     # Unused
    sel : 4         # Operation to perform, one of ALU_SEL_xxx
    sub_opcode : 3  # Sub opcode (SUB_OPCODE_ALU_REG)
    opcode : 4      # Opcode (OPCODE_ALU)
""")


_alu_imm = make_ins("""
    dreg : 2        # Destination register
    sreg : 2        # Register with operand A
    imm : 16        # Immediate value of operand B
    unused : 1      # Unused
    sel : 4         # Operation to perform, one of ALU_SEL_xxx
    sub_opcode : 3  # Sub opcode (SUB_OPCODE_ALU_IMM)
    opcode : 4      # Opcode (OPCODE_ALU)
""")


_alu_cnt = make_ins("""
    unused1 : 4     # Unused
    imm : 8         # Immediate value (to inc / dec stage counter)
    unused2 : 9     # Unused
    sel : 4         # Operation to perform, one of ALU_SEL_xxx
    sub_opcode : 3  # Sub opcode (SUB_OPCODE_ALU_CNT)
    opcode : 4      # Opcode (OPCODE_ALU)
""")


_bx = make_ins("""
    dreg : 2        # Register which contains target PC, expressed in words (used if .reg == 1)
    addr : 11       # Target PC, expressed in words (used if .reg == 0)
    unused : 8      # Unused
    reg : 1         # Target PC in register (1) or immediate (0)
    type : 3        # Jump condition (BX_JUMP_TYPE_xxx)
    sub_opcode : 3  # Sub opcode (SUB_OPCODE_BX)
    opcode : 4      # Opcode (OPCODE_BRANCH)
""")


_br = make_ins("""
    imm : 16        # Immediate value to compare against
    cmp : 1         # Comparison to perform: BRCOND_LT or BRCOND_GE
    offset : 7      # Absolute value of target PC offset w.r.t. current PC, expressed in words
    sign : 1        # Sign of target PC offset: 0: positive, 1: negative
    sub_opcode : 3  # Sub opcode (SUB_OPCODE_BR)
    opcode : 4      # Opcode (OPCODE_BRANCH)
""")


_bs = make_ins("""
    imm : 8         # Immediate value to compare against
    unused : 7      # Unused
    cmp : 2         # Comparison to perform: BRCOND_LT, GT or EQ
    offset : 7      # Absolute value of target PC offset w.r.t. current PC, expressed in words
    sign : 1        # Sign of target PC offset: 0: positive, 1: negative
    sub_opcode : 3  # Sub opcode (SUB_OPCODE_BS)
    opcode : 4      # Opcode (OPCODE_BRANCH)
""")


_end = make_ins("""
    wakeup : 1      # Set to 1 to wake up chip
    unused : 24     # Unused
    sub_opcode : 3  # Sub opcode (SUB_OPCODE_END)
    opcode : 4      # Opcode (OPCODE_END)
""")


_sleep = make_ins("""
    cycle_sel : 4   # Select which one of SARADC_ULP_CP_SLEEP_CYCx_REG to get the sleep duration from
    unused : 21     # Unused
    sub_opcode : 3  # Sub opcode (SUB_OPCODE_SLEEP)
    opcode : 4      # Opcode (OPCODE_END)
""")


_halt = make_ins("""
    unused : 28     # Unused
    opcode : 4      # Opcode (OPCODE_HALT)
""")


_ld = make_ins("""
    dreg : 2        # Register where the data should be loaded to
    sreg : 2        # Register which contains address in RTC memory (expressed in words)
    unused1 : 6     # Unused
    offset : 11     # Offset to add to sreg
    unused2 : 7     # Unused
    opcode : 4      # Opcode (OPCODE_LD)
""")


# assembler opcode definitions

REG, IMM, COND, SYM = 0, 1, 2, 3
ARG = namedtuple('ARG', ('type', 'value', 'raw'))


def eval_arg(arg):
    parts = []
    for token in split_tokens(arg):
        if symbols.has_sym(token):
            _, _, sym_value = symbols.get_sym(token)
            parts.append(str(sym_value))
        else:
            parts.append(token)
    parts = "".join(parts)
    if not validate_expression(parts):
        raise ValueError('Unsupported expression: %s' % parts)
    return eval(parts)


def arg_qualify(arg):
    """
    look at arg and qualify its type:
    REG(ister), IMM(ediate) value

    then convert arg into a int value, e.g. 'R1' -> 1 or '0x20' -> 32.

    return result as ARG namedtuple
    """
    arg_lower = arg.lower()
    if len(arg) == 2:
        if arg_lower[0] == 'r' and arg[1] in '0123456789':
            reg = int(arg[1])
            if 0 <= reg <= 3:
                return ARG(REG, reg, arg)
            raise ValueError('arg_qualify: valid registers are r0, r1, r2, r3. Given: %s' % arg)
        if arg_lower in ['--', 'eq', 'ov', 'lt', 'gt', 'ge', 'le']:
            return ARG(COND, arg_lower, arg)
    try:
        return ARG(IMM, int(arg), arg)
    except ValueError:
        pass
    try:
        entry = symbols.get_sym(arg)
    except KeyError:
        return ARG(IMM, int(eval_arg(arg)), arg)
    else:
        return ARG(SYM, entry, arg)


def get_reg(arg):
    if isinstance(arg, str):
        arg = arg_qualify(arg)
    if arg.type == REG:
        return arg.value
    raise TypeError('wanted: register, got: %s' % arg.raw)


def get_imm(arg):
    if isinstance(arg, str):
        arg = arg_qualify(arg)
    if arg.type == IMM:
        return arg.value
    if arg.type == SYM:
        return symbols.resolve_absolute(arg.value)
    raise TypeError('wanted: immediate, got: %s' % arg.raw)


get_abs = get_imm


def get_rel(arg):
    if isinstance(arg, str):
        arg = arg_qualify(arg)
    if arg.type == IMM:
        if arg.value & 3 != 0:  # bitwise version of: arg.value % 4 != 0
            raise ValueError('Relative offset must be a multiple of 4')
        return IMM, arg.value >> 2  # bitwise version of: arg.value // 4
    if arg.type == SYM:
        return SYM, symbols.resolve_relative(arg.value)
    raise TypeError('wanted: immediate, got: %s' % arg.raw)


def get_cond(arg):
    if isinstance(arg, str):
        arg = arg_qualify(arg)
    if arg.type == COND:
        return arg.value
    raise TypeError('wanted: condition, got: %s' % arg.raw)


def _soc_reg_to_ulp_periph_sel(reg):
    # Map SoC peripheral register to periph_sel field of RD_REG and WR_REG instructions.
    if reg < DR_REG_RTCCNTL_BASE:
        raise ValueError("invalid register base")
    elif reg < DR_REG_RTCIO_BASE:
        ret = RD_REG_PERIPH_RTC_CNTL
    elif reg < DR_REG_SENS_BASE:
        ret = RD_REG_PERIPH_RTC_IO
    elif reg < DR_REG_RTC_I2C_BASE:
        ret = RD_REG_PERIPH_SENS
    elif reg < DR_REG_IO_MUX_BASE:
        ret = RD_REG_PERIPH_RTC_I2C
    else:
        raise ValueError("invalid register base")
    return ret


def i_reg_wr(reg, high_bit, low_bit, val):
    reg = get_imm(reg)
    if reg <= DR_REG_MAX_DIRECT:  # see https://github.com/espressif/binutils-esp32ulp/blob/master/gas/config/tc-esp32ulp_esp32.c
        _wr_reg.addr = reg & 0xff
        _wr_reg.periph_sel = (reg & 0x300) >> 8
    else:
        _wr_reg.addr = (reg & 0xff) >> 2
        _wr_reg.periph_sel = _soc_reg_to_ulp_periph_sel(reg)
    _wr_reg.data = get_imm(val)
    _wr_reg.low = get_imm(low_bit)
    _wr_reg.high = get_imm(high_bit)
    _wr_reg.opcode = OPCODE_WR_REG
    return _wr_reg.all


def i_reg_rd(reg, high_bit, low_bit):
    reg = get_imm(reg)
    if reg <= DR_REG_MAX_DIRECT:  # see https://github.com/espressif/binutils-esp32ulp/blob/master/gas/config/tc-esp32ulp_esp32.c
        _rd_reg.addr = reg & 0xff
        _rd_reg.periph_sel = (reg & 0x300) >> 8
    else:
        _rd_reg.addr = (reg & 0xff) >> 2
        _rd_reg.periph_sel = _soc_reg_to_ulp_periph_sel(reg)
    _rd_reg.unused = 0
    _rd_reg.low = get_imm(low_bit)
    _rd_reg.high = get_imm(high_bit)
    _rd_reg.opcode = OPCODE_RD_REG
    return _rd_reg.all


def i_i2c_rd(sub_addr, high_bit, low_bit, slave_sel):
    _i2c.sub_addr = get_imm(sub_addr)
    _i2c.data = 0
    _i2c.low = get_imm(low_bit)
    _i2c.high = get_imm(high_bit)
    _i2c.i2c_sel = get_imm(slave_sel)
    _i2c.unused = 0
    _i2c.rw = 0
    _i2c.opcode = OPCODE_I2C
    return _i2c.all


def i_i2c_wr(sub_addr, value, high_bit, low_bit, slave_sel):
    _i2c.sub_addr = get_imm(sub_addr)
    _i2c.data = get_imm(value)
    _i2c.low = get_imm(low_bit)
    _i2c.high = get_imm(high_bit)
    _i2c.i2c_sel = get_imm(slave_sel)
    _i2c.unused = 0
    _i2c.rw = 1
    _i2c.opcode = OPCODE_I2C
    return _i2c.all


def i_nop():
    _delay.cycles = 0
    _delay.unused = 0
    _delay.opcode = OPCODE_DELAY
    return _delay.all


def i_wait(cycles):
    _delay.cycles = get_imm(cycles)
    _delay.unused = 0
    _delay.opcode = OPCODE_DELAY
    return _delay.all


def i_tsens(reg_dest, delay):
    _tsens.dreg = get_reg(reg_dest)
    _tsens.delay = get_imm(delay)
    _tsens.unused = 0
    _tsens.opcode = OPCODE_TSENS
    return _tsens.all


def i_adc(reg_dest, adc_idx, mux, _not_used=None):
    _adc.dreg = get_reg(reg_dest)
    _adc.mux = get_imm(mux)
    _adc.sar_sel = get_imm(adc_idx)
    _adc.unused1 = 0
    _adc.cycles = 0
    _adc.unused2 = 0
    _adc.opcode = OPCODE_ADC
    return _adc.all


def i_st(reg_val, reg_addr, offset):
    _st.dreg = get_reg(reg_addr)
    _st.sreg = get_reg(reg_val)
    _st.unused1 = 0
    _st.offset = get_imm(offset) // 4
    _st.unused2 = 0
    _st.sub_opcode = SUB_OPCODE_ST
    _st.opcode = OPCODE_ST
    return _st.all


def i_halt():
    _halt.unused = 0
    _halt.opcode = OPCODE_HALT
    return _halt.all


def i_ld(reg_dest, reg_addr, offset):
    _ld.dreg = get_reg(reg_dest)
    _ld.sreg = get_reg(reg_addr)
    _ld.unused1 = 0
    _ld.offset = get_imm(offset) // 4
    _ld.unused2 = 0
    _ld.opcode = OPCODE_LD
    return _ld.all


def i_move(reg_dest, reg_imm_src):
    # this is the only ALU instruction with 2 args: move r0, r1
    dest = get_reg(reg_dest)
    src = arg_qualify(reg_imm_src)
    if src.type == REG:
        _alu_reg.dreg = dest
        _alu_reg.sreg = src.value
        _alu_reg.treg = src.value  # XXX undocumented, this is the value binutils-esp32 uses
        _alu_reg.unused = 0
        _alu_reg.sel = ALU_SEL_MOV
        _alu_reg.sub_opcode = SUB_OPCODE_ALU_REG
        _alu_reg.opcode = OPCODE_ALU
        return _alu_reg.all
    if src.type == IMM or src.type == SYM:
        _alu_imm.dreg = dest
        _alu_imm.sreg = 0
        _alu_imm.imm = get_abs(src)
        _alu_imm.unused = 0
        _alu_imm.sel = ALU_SEL_MOV
        _alu_imm.sub_opcode = SUB_OPCODE_ALU_IMM
        _alu_imm.opcode = OPCODE_ALU
        return _alu_imm.all
    raise TypeError('unsupported operand: %s' % src.raw)


def _alu3(reg_dest, reg_src1, reg_imm_src2, alu_sel):
    """
    ALU instructions with 3 args, like e.g. add r1, r2, r3
    """
    dest = get_reg(reg_dest)
    src1 = get_reg(reg_src1)
    src2 = arg_qualify(reg_imm_src2)
    if src2.type == REG:
        _alu_reg.dreg = dest
        _alu_reg.sreg = src1
        _alu_reg.treg = src2.value
        _alu_reg.unused = 0
        _alu_reg.sel = alu_sel
        _alu_reg.sub_opcode = SUB_OPCODE_ALU_REG
        _alu_reg.opcode = OPCODE_ALU
        return _alu_reg.all
    if src2.type == IMM or src2.type == SYM:
        _alu_imm.dreg = dest
        _alu_imm.sreg = src1
        _alu_imm.imm = get_abs(src2)
        _alu_imm.unused = 0
        _alu_imm.sel = alu_sel
        _alu_imm.sub_opcode = SUB_OPCODE_ALU_IMM
        _alu_imm.opcode = OPCODE_ALU
        return _alu_imm.all
    raise TypeError('unsupported operand: %s' % src2.raw)


def i_add(reg_dest, reg_src1, reg_imm_src2):
    return _alu3(reg_dest, reg_src1, reg_imm_src2, ALU_SEL_ADD)


def i_sub(reg_dest, reg_src1, reg_imm_src2):
    return _alu3(reg_dest, reg_src1, reg_imm_src2, ALU_SEL_SUB)


def i_and(reg_dest, reg_src1, reg_imm_src2):
    return _alu3(reg_dest, reg_src1, reg_imm_src2, ALU_SEL_AND)


def i_or(reg_dest, reg_src1, reg_imm_src2):
    return _alu3(reg_dest, reg_src1, reg_imm_src2, ALU_SEL_OR)


def i_lsh(reg_dest, reg_src1, reg_imm_src2):
    return _alu3(reg_dest, reg_src1, reg_imm_src2, ALU_SEL_LSH)


def i_rsh(reg_dest, reg_src1, reg_imm_src2):
    return _alu3(reg_dest, reg_src1, reg_imm_src2, ALU_SEL_RSH)


def _alu_stage(imm, alu_sel):
    """
    Stage counter instructions with 1 arg: stage_inc / stage_dec
    """
    imm = get_imm(imm)
    _alu_cnt.unused1 = 0
    _alu_cnt.imm = imm
    _alu_cnt.unused2 = 0
    _alu_cnt.sel = alu_sel
    _alu_cnt.sub_opcode = SUB_OPCODE_ALU_CNT
    _alu_cnt.opcode = OPCODE_ALU
    return _alu_cnt.all


def i_stage_inc(imm):
    return _alu_stage(imm, ALU_SEL_INC)


def i_stage_dec(imm):
    return _alu_stage(imm, ALU_SEL_DEC)


def i_stage_rst():
    return _alu_stage('0', ALU_SEL_RST)


def i_wake():
    _end.wakeup = 1
    _end.unused = 0
    _end.sub_opcode = SUB_OPCODE_END
    _end.opcode = OPCODE_END
    return _end.all


def i_sleep(timer_idx):
    _sleep.cycle_sel = get_imm(timer_idx)
    _sleep.unused = 0
    _sleep.sub_opcode = SUB_OPCODE_SLEEP
    _sleep.opcode = OPCODE_END
    return _sleep.all


def i_jump(target, condition='--'):
    target = arg_qualify(target)
    condition = get_cond(condition)
    if condition == 'eq':
        jump_type = BX_JUMP_TYPE_ZERO
    elif condition == 'ov':
        jump_type = BX_JUMP_TYPE_OVF
    elif condition == '--':  # means unconditional
        jump_type = BX_JUMP_TYPE_DIRECT
    else:
        raise ValueError("invalid flags condition")
    if target.type == IMM or target.type == SYM:
        _bx.dreg = 0
        # we track label addresses in 32bit words, but immediate values are in bytes and need to get divided by 4.
        _bx.addr = get_abs(target) if target.type == SYM else get_abs(target) >> 2  # bitwise version of "// 4"
        _bx.unused = 0
        _bx.reg = 0
        _bx.type = jump_type
        _bx.sub_opcode = SUB_OPCODE_BX
        _bx.opcode = OPCODE_BRANCH
        return _bx.all
    if target.type == REG:
        _bx.dreg = target.value
        _bx.addr = 0
        _bx.unused = 0
        _bx.reg = 1
        _bx.type = jump_type
        _bx.sub_opcode = SUB_OPCODE_BX
        _bx.opcode = OPCODE_BRANCH
        return _bx.all
    raise TypeError('unsupported operand: %s' % target.raw)


def _jump_relr(threshold, cond, offset):
    """
    Equivalent of I_JUMP_RELR macro in binutils-esp32ulp
    """
    _br.imm = threshold
    _br.cmp = cond
    _br.offset = abs(offset)
    _br.sign = 0 if offset >= 0 else 1
    _br.sub_opcode = SUB_OPCODE_BR
    _br.opcode = OPCODE_BRANCH
    return _br.all


def i_jumpr(offset, threshold, condition):
    offset_type, offset = get_rel(offset)
    threshold = get_imm(threshold)
    condition = get_cond(condition)
    if condition == 'lt':
        cmp_op = BRCOND_LT
    elif condition == 'ge':
        cmp_op = BRCOND_GE
    elif condition == 'le':  # le == lt(threshold+1)
        threshold += 1
        cmp_op = BRCOND_LT
    elif condition == 'gt':  # gt == ge(threshold+1)
        threshold += 1
        cmp_op = BRCOND_GE
    elif condition == 'eq':  # eq == ge(threshold) but not ge(threshold+1)
        # jump over next JUMPR
        skip_ins = _jump_relr(threshold + 1, BRCOND_GE, 2)
        # jump to target
        if (offset_type == IMM and offset < 0) or offset_type == SYM:
            # adjust for the additional JUMPR instruction
            # for IMM offsets, the offset is relative to the 2nd instruction, so only backwards jumps need adjusting
            # for SYM offsets, label offsets already include the extra instruction, so both directions need adjusting
            offset -= 1
        jump_ins = _jump_relr(threshold, BRCOND_GE, offset)
        return (skip_ins, jump_ins)
    else:
        raise ValueError("invalid comparison condition")
    return _jump_relr(threshold, cmp_op, offset)


def _jump_rels(threshold, cond, offset):
    """
    Equivalent of I_JUMP_RELS macro in binutils-esp32ulp
    """
    _bs.imm = threshold
    _bs.cmp = cond
    _bs.offset = abs(offset)
    _bs.sign = 0 if offset >= 0 else 1
    _bs.sub_opcode = SUB_OPCODE_BS
    _bs.opcode = OPCODE_BRANCH
    return _bs.all


def i_jumps(offset, threshold, condition):
    offset_type, offset = get_rel(offset)
    threshold = get_imm(threshold)
    condition = get_cond(condition)
    if condition == 'lt':
        cmp_op = BRCOND_LT
    elif condition == 'le':
        cmp_op = BRCOND_LE
    elif condition == 'ge':
        cmp_op = BRCOND_GE
    elif condition in ('eq', 'gt'):
        if condition == 'eq':  # eq == le but not lt
            skip_cond = BRCOND_LT
            jump_cond = BRCOND_LE
        elif condition == 'gt':  # gt == ge but not le
            skip_cond = BRCOND_LE
            jump_cond = BRCOND_GE

        # jump over next JUMPS
        skip_ins = _jump_rels(threshold, skip_cond, 2)
        # jump to target
        if (offset_type == IMM and offset < 0) or offset_type == SYM:
            # adjust for the additional JUMPS instruction
            # for IMM offsets, the offset is relative to the 2nd instruction, so only backwards jumps need adjusting
            # for SYM offsets, label offsets already include the extra instruction, so both directions need adjusting
            offset -= 1
        jump_ins = _jump_rels(threshold, jump_cond, offset)

        return (skip_ins, jump_ins)
    else:
        raise ValueError("invalid comparison condition")
    return _jump_rels(threshold, cmp_op, offset)


def no_of_instr(opcode, args):
    if opcode == 'jumpr' and get_cond(args[2]) == 'eq':
        return 2

    if opcode == 'jumps' and get_cond(args[2]) in ('eq', 'gt'):
        return 2

    return 1
