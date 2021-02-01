#!/usr/bin/env python3
#
# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2019 Damien P. George
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

"""
Link .o files to .mpy
"""

import sys, os, struct, re
from elftools.elf import elffile

sys.path.append(os.path.dirname(__file__) + "/../py")
import makeqstrdata as qstrutil

# MicroPython constants
MPY_VERSION = 5
MP_NATIVE_ARCH_X86 = 1
MP_NATIVE_ARCH_X64 = 2
MP_NATIVE_ARCH_ARMV7M = 5
MP_NATIVE_ARCH_ARMV7EMSP = 7
MP_NATIVE_ARCH_ARMV7EMDP = 8
MP_NATIVE_ARCH_XTENSA = 9
MP_NATIVE_ARCH_XTENSAWIN = 10
MP_CODE_BYTECODE = 2
MP_CODE_NATIVE_VIPER = 4
MP_SCOPE_FLAG_VIPERRELOC = 0x10
MP_SCOPE_FLAG_VIPERRODATA = 0x20
MP_SCOPE_FLAG_VIPERBSS = 0x40
MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE = 1
MICROPY_PY_BUILTINS_STR_UNICODE = 2
MP_SMALL_INT_BITS = 31
QSTR_WINDOW_SIZE = 32

# ELF constants
R_386_32 = 1
R_X86_64_64 = 1
R_XTENSA_32 = 1
R_386_PC32 = 2
R_X86_64_PC32 = 2
R_ARM_ABS32 = 2
R_386_GOT32 = 3
R_ARM_REL32 = 3
R_386_PLT32 = 4
R_X86_64_PLT32 = 4
R_XTENSA_PLT = 6
R_386_GOTOFF = 9
R_386_GOTPC = 10
R_ARM_THM_CALL = 10
R_XTENSA_DIFF32 = 19
R_XTENSA_SLOT0_OP = 20
R_ARM_BASE_PREL = 25  # aka R_ARM_GOTPC
R_ARM_GOT_BREL = 26  # aka R_ARM_GOT32
R_ARM_THM_JUMP24 = 30
R_X86_64_REX_GOTPCRELX = 42
R_386_GOT32X = 43

################################################################################
# Architecture configuration


def asm_jump_x86(entry):
    return struct.pack("<BI", 0xE9, entry - 5)


def asm_jump_arm(entry):
    b_off = entry - 4
    if b_off >> 11 == 0 or b_off >> 11 == -1:
        # Signed value fits in 12 bits
        b0 = 0xE000 | (b_off >> 1 & 0x07FF)
        b1 = 0
    else:
        # Use large jump
        b0 = 0xF000 | (b_off >> 12 & 0x07FF)
        b1 = 0xB800 | (b_off >> 1 & 0x7FF)
    return struct.pack("<HH", b0, b1)


def asm_jump_xtensa(entry):
    jump_offset = entry - 4
    jump_op = jump_offset << 6 | 6
    return struct.pack("<BH", jump_op & 0xFF, jump_op >> 8)


class ArchData:
    def __init__(self, name, mpy_feature, qstr_entry_size, word_size, arch_got, asm_jump):
        self.name = name
        self.mpy_feature = mpy_feature
        self.qstr_entry_size = qstr_entry_size
        self.word_size = word_size
        self.arch_got = arch_got
        self.asm_jump = asm_jump
        self.separate_rodata = name == "EM_XTENSA" and qstr_entry_size == 4


ARCH_DATA = {
    "x86": ArchData(
        "EM_386",
        MP_NATIVE_ARCH_X86 << 2
        | MICROPY_PY_BUILTINS_STR_UNICODE
        | MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE,
        2,
        4,
        (R_386_PC32, R_386_GOT32, R_386_GOT32X),
        asm_jump_x86,
    ),
    "x64": ArchData(
        "EM_X86_64",
        MP_NATIVE_ARCH_X64 << 2
        | MICROPY_PY_BUILTINS_STR_UNICODE
        | MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE,
        2,
        8,
        (R_X86_64_REX_GOTPCRELX,),
        asm_jump_x86,
    ),
    "armv7m": ArchData(
        "EM_ARM",
        MP_NATIVE_ARCH_ARMV7M << 2 | MICROPY_PY_BUILTINS_STR_UNICODE,
        2,
        4,
        (R_ARM_GOT_BREL,),
        asm_jump_arm,
    ),
    "armv7emsp": ArchData(
        "EM_ARM",
        MP_NATIVE_ARCH_ARMV7EMSP << 2 | MICROPY_PY_BUILTINS_STR_UNICODE,
        2,
        4,
        (R_ARM_GOT_BREL,),
        asm_jump_arm,
    ),
    "armv7emdp": ArchData(
        "EM_ARM",
        MP_NATIVE_ARCH_ARMV7EMDP << 2 | MICROPY_PY_BUILTINS_STR_UNICODE,
        2,
        4,
        (R_ARM_GOT_BREL,),
        asm_jump_arm,
    ),
    "xtensa": ArchData(
        "EM_XTENSA",
        MP_NATIVE_ARCH_XTENSA << 2 | MICROPY_PY_BUILTINS_STR_UNICODE,
        2,
        4,
        (R_XTENSA_32, R_XTENSA_PLT),
        asm_jump_xtensa,
    ),
    "xtensawin": ArchData(
        "EM_XTENSA",
        MP_NATIVE_ARCH_XTENSAWIN << 2 | MICROPY_PY_BUILTINS_STR_UNICODE,
        4,
        4,
        (R_XTENSA_32, R_XTENSA_PLT),
        asm_jump_xtensa,
    ),
}

################################################################################
# Helper functions


def align_to(value, align):
    return (value + align - 1) & ~(align - 1)


def unpack_u24le(data, offset):
    return data[offset] | data[offset + 1] << 8 | data[offset + 2] << 16


def pack_u24le(data, offset, value):
    data[offset] = value & 0xFF
    data[offset + 1] = value >> 8 & 0xFF
    data[offset + 2] = value >> 16 & 0xFF


def xxd(text):
    for i in range(0, len(text), 16):
        print("{:08x}:".format(i), end="")
        for j in range(4):
            off = i + j * 4
            if off < len(text):
                d = int.from_bytes(text[off : off + 4], "little")
                print(" {:08x}".format(d), end="")
        print()


# Smaller numbers are enabled first
LOG_LEVEL_1 = 1
LOG_LEVEL_2 = 2
LOG_LEVEL_3 = 3
log_level = LOG_LEVEL_1


def log(level, msg):
    if level <= log_level:
        print(msg)


################################################################################
# Qstr extraction


def extract_qstrs(source_files):
    def read_qstrs(f):
        with open(f) as f:
            vals = set()
            objs = set()
            for line in f:
                while line:
                    m = re.search(r"MP_OBJ_NEW_QSTR\((MP_QSTR_[A-Za-z0-9_]*)\)", line)
                    if m:
                        objs.add(m.group(1))
                    else:
                        m = re.search(r"MP_QSTR_[A-Za-z0-9_]*", line)
                        if m:
                            vals.add(m.group())
                    if m:
                        s = m.span()
                        line = line[: s[0]] + line[s[1] :]
                    else:
                        line = ""
            return vals, objs

    static_qstrs = ["MP_QSTR_" + qstrutil.qstr_escape(q) for q in qstrutil.static_qstr_list]

    qstr_vals = set()
    qstr_objs = set()
    for f in source_files:
        vals, objs = read_qstrs(f)
        qstr_vals.update(vals)
        qstr_objs.update(objs)
    qstr_vals.difference_update(static_qstrs)

    return static_qstrs, qstr_vals, qstr_objs


################################################################################
# Linker


class LinkError(Exception):
    pass


class Section:
    def __init__(self, name, data, alignment, filename=None):
        self.filename = filename
        self.name = name
        self.data = data
        self.alignment = alignment
        self.addr = 0
        self.reloc = []

    @staticmethod
    def from_elfsec(elfsec, filename):
        assert elfsec.header.sh_addr == 0
        return Section(elfsec.name, elfsec.data(), elfsec.data_alignment, filename)


class GOTEntry:
    def __init__(self, name, sym, link_addr=0):
        self.name = name
        self.sym = sym
        self.offset = None
        self.link_addr = link_addr

    def isexternal(self):
        return self.sec_name.startswith(".external")

    def istext(self):
        return self.sec_name.startswith(".text")

    def isrodata(self):
        return self.sec_name.startswith((".rodata", ".data.rel.ro"))

    def isbss(self):
        return self.sec_name.startswith(".bss")


class LiteralEntry:
    def __init__(self, value, offset):
        self.value = value
        self.offset = offset


class LinkEnv:
    def __init__(self, arch):
        self.arch = ARCH_DATA[arch]
        self.sections = []  # list of sections in order of output
        self.literal_sections = []  # list of literal sections (xtensa only)
        self.known_syms = {}  # dict of symbols that are defined
        self.unresolved_syms = []  # list of unresolved symbols
        self.mpy_relocs = []  # list of relocations needed in the output .mpy file

    def check_arch(self, arch_name):
        if arch_name != self.arch.name:
            raise LinkError("incompatible arch")

    def print_sections(self):
        log(LOG_LEVEL_2, "sections:")
        for sec in self.sections:
            log(LOG_LEVEL_2, "  {:08x} {} size={}".format(sec.addr, sec.name, len(sec.data)))

    def find_addr(self, name):
        if name in self.known_syms:
            s = self.known_syms[name]
            return s.section.addr + s["st_value"]
        raise LinkError("unknown symbol: {}".format(name))


def build_got_generic(env):
    env.got_entries = {}
    for sec in env.sections:
        for r in sec.reloc:
            s = r.sym
            if not (
                s.entry["st_info"]["bind"] == "STB_GLOBAL"
                and r["r_info_type"] in env.arch.arch_got
            ):
                continue
            s_type = s.entry["st_info"]["type"]
            assert s_type in ("STT_NOTYPE", "STT_FUNC", "STT_OBJECT"), s_type
            assert s.name
            if s.name in env.got_entries:
                continue
            env.got_entries[s.name] = GOTEntry(s.name, s)


def build_got_xtensa(env):
    env.got_entries = {}
    env.lit_entries = {}
    env.xt_literals = {}

    # Extract the values from the literal table
    for sec in env.literal_sections:
        assert len(sec.data) % env.arch.word_size == 0

        # Look through literal relocations to find any global pointers that should be GOT entries
        for r in sec.reloc:
            s = r.sym
            s_type = s.entry["st_info"]["type"]
            assert s_type in ("STT_NOTYPE", "STT_FUNC", "STT_OBJECT", "STT_SECTION"), s_type
            assert r["r_info_type"] in env.arch.arch_got
            assert r["r_offset"] % env.arch.word_size == 0
            # This entry is a global pointer
            existing = struct.unpack_from("<I", sec.data, r["r_offset"])[0]
            if s_type == "STT_SECTION":
                assert r["r_addend"] == 0
                name = "{}+0x{:x}".format(s.section.name, existing)
            else:
                assert existing == 0
                name = s.name
                if r["r_addend"] != 0:
                    name = "{}+0x{:x}".format(name, r["r_addend"])
            idx = "{}+0x{:x}".format(sec.filename, r["r_offset"])
            env.xt_literals[idx] = name
            if name in env.got_entries:
                # Deduplicate GOT entries
                continue
            env.got_entries[name] = GOTEntry(name, s, existing)

        # Go through all literal entries finding those that aren't global pointers so must be actual literals
        for i in range(0, len(sec.data), env.arch.word_size):
            idx = "{}+0x{:x}".format(sec.filename, i)
            if idx not in env.xt_literals:
                # This entry is an actual literal
                value = struct.unpack_from("<I", sec.data, i)[0]
                env.xt_literals[idx] = value
                if value in env.lit_entries:
                    # Deduplicate literals
                    continue
                env.lit_entries[value] = LiteralEntry(
                    value, len(env.lit_entries) * env.arch.word_size
                )


def populate_got(env):
    # Compute GOT destination addresses
    for got_entry in env.got_entries.values():
        sym = got_entry.sym
        if hasattr(sym, "resolved"):
            sym = sym.resolved
        sec = sym.section
        addr = sym["st_value"]
        got_entry.sec_name = sec.name
        got_entry.link_addr += sec.addr + addr

    # Get sorted GOT, sorted by external, text, rodata, bss so relocations can be combined
    got_list = sorted(
        env.got_entries.values(),
        key=lambda g: g.isexternal() + 2 * g.istext() + 3 * g.isrodata() + 4 * g.isbss(),
    )

    # Layout and populate the GOT
    offset = 0
    for got_entry in got_list:
        got_entry.offset = offset
        offset += env.arch.word_size
        o = env.got_section.addr + got_entry.offset
        env.full_text[o : o + env.arch.word_size] = got_entry.link_addr.to_bytes(
            env.arch.word_size, "little"
        )

    # Create a relocation for each GOT entry
    for got_entry in got_list:
        if got_entry.name == "mp_fun_table":
            dest = "mp_fun_table"
        elif got_entry.name.startswith("mp_fun_table+0x"):
            dest = int(got_entry.name.split("+")[1], 16) // env.arch.word_size
        elif got_entry.sec_name.startswith(".text"):
            dest = ".text"
        elif got_entry.sec_name.startswith(".rodata"):
            dest = ".rodata"
        elif got_entry.sec_name.startswith(".data.rel.ro"):
            dest = ".data.rel.ro"
        elif got_entry.sec_name.startswith(".bss"):
            dest = ".bss"
        else:
            assert 0, (got_entry.name, got_entry.sec_name)
        env.mpy_relocs.append((".text", env.got_section.addr + got_entry.offset, dest))

    # Print out the final GOT
    log(LOG_LEVEL_2, "GOT: {:08x}".format(env.got_section.addr))
    for g in got_list:
        log(
            LOG_LEVEL_2,
            "  {:08x} {} -> {}+{:08x}".format(g.offset, g.name, g.sec_name, g.link_addr),
        )


def populate_lit(env):
    log(LOG_LEVEL_2, "LIT: {:08x}".format(env.lit_section.addr))
    for lit_entry in env.lit_entries.values():
        value = lit_entry.value
        log(LOG_LEVEL_2, "  {:08x} = {:08x}".format(lit_entry.offset, value))
        o = env.lit_section.addr + lit_entry.offset
        env.full_text[o : o + env.arch.word_size] = value.to_bytes(env.arch.word_size, "little")


def do_relocation_text(env, text_addr, r):
    # Extract relevant info about symbol that's being relocated
    s = r.sym
    s_bind = s.entry["st_info"]["bind"]
    s_shndx = s.entry["st_shndx"]
    s_type = s.entry["st_info"]["type"]
    r_offset = r["r_offset"] + text_addr
    r_info_type = r["r_info_type"]
    try:
        # only for RELA sections
        r_addend = r["r_addend"]
    except KeyError:
        r_addend = 0

    # Default relocation type and name for logging
    reloc_type = "le32"
    log_name = None

    if (
        env.arch.name == "EM_386"
        and r_info_type in (R_386_PC32, R_386_PLT32)
        or env.arch.name == "EM_X86_64"
        and r_info_type in (R_X86_64_PC32, R_X86_64_PLT32)
        or env.arch.name == "EM_ARM"
        and r_info_type in (R_ARM_REL32, R_ARM_THM_CALL, R_ARM_THM_JUMP24)
        or s_bind == "STB_LOCAL"
        and env.arch.name == "EM_XTENSA"
        and r_info_type == R_XTENSA_32  # not GOT
    ):
        # Standard relocation to fixed location within text/rodata
        if hasattr(s, "resolved"):
            s = s.resolved

        sec = s.section

        if env.arch.separate_rodata and sec.name.startswith(".rodata"):
            raise LinkError("fixed relocation to rodata with rodata referenced via GOT")

        if sec.name.startswith(".bss"):
            raise LinkError(
                "{}: fixed relocation to bss (bss variables can't be static)".format(s.filename)
            )

        if sec.name.startswith(".external"):
            raise LinkError(
                "{}: fixed relocation to external symbol: {}".format(s.filename, s.name)
            )

        addr = sec.addr + s["st_value"]
        reloc = addr - r_offset + r_addend

        if r_info_type in (R_ARM_THM_CALL, R_ARM_THM_JUMP24):
            # Both relocations have the same bit pattern to rewrite:
            #   R_ARM_THM_CALL: bl
            #   R_ARM_THM_JUMP24: b.w
            reloc_type = "thumb_b"

    elif (
        env.arch.name == "EM_386"
        and r_info_type == R_386_GOTPC
        or env.arch.name == "EM_ARM"
        and r_info_type == R_ARM_BASE_PREL
    ):
        # Relocation to GOT address itself
        assert s.name == "_GLOBAL_OFFSET_TABLE_"
        addr = env.got_section.addr
        reloc = addr - r_offset + r_addend

    elif (
        env.arch.name == "EM_386"
        and r_info_type in (R_386_GOT32, R_386_GOT32X)
        or env.arch.name == "EM_ARM"
        and r_info_type == R_ARM_GOT_BREL
    ):
        # Relcation pointing to GOT
        reloc = addr = env.got_entries[s.name].offset

    elif env.arch.name == "EM_X86_64" and r_info_type == R_X86_64_REX_GOTPCRELX:
        # Relcation pointing to GOT
        got_entry = env.got_entries[s.name]
        addr = env.got_section.addr + got_entry.offset
        reloc = addr - r_offset + r_addend

    elif env.arch.name == "EM_386" and r_info_type == R_386_GOTOFF:
        # Relocation relative to GOT
        addr = s.section.addr + s["st_value"]
        reloc = addr - env.got_section.addr + r_addend

    elif env.arch.name == "EM_XTENSA" and r_info_type == R_XTENSA_SLOT0_OP:
        # Relocation pointing to GOT, xtensa specific
        sec = s.section
        if sec.name.startswith(".text"):
            # it looks like R_XTENSA_SLOT0_OP into .text is already correctly relocated
            return
        assert sec.name.startswith(".literal"), sec.name
        lit_idx = "{}+0x{:x}".format(sec.filename, r_addend)
        lit_ptr = env.xt_literals[lit_idx]
        if isinstance(lit_ptr, str):
            addr = env.got_section.addr + env.got_entries[lit_ptr].offset
            log_name = "GOT {}".format(lit_ptr)
        else:
            addr = env.lit_section.addr + env.lit_entries[lit_ptr].offset
            log_name = "LIT"
        reloc = addr - r_offset
        reloc_type = "xtensa_l32r"

    elif env.arch.name == "EM_XTENSA" and r_info_type == R_XTENSA_DIFF32:
        if s.section.name.startswith(".text"):
            # it looks like R_XTENSA_DIFF32 into .text is already correctly relocated
            return
        assert 0

    else:
        # Unknown/unsupported relocation
        assert 0, r_info_type

    # Write relocation
    if reloc_type == "le32":
        (existing,) = struct.unpack_from("<I", env.full_text, r_offset)
        struct.pack_into("<I", env.full_text, r_offset, (existing + reloc) & 0xFFFFFFFF)
    elif reloc_type == "thumb_b":
        b_h, b_l = struct.unpack_from("<HH", env.full_text, r_offset)
        existing = (b_h & 0x7FF) << 12 | (b_l & 0x7FF) << 1
        if existing >= 0x400000:  # 2's complement
            existing -= 0x800000
        new = existing + reloc
        b_h = (b_h & 0xF800) | (new >> 12) & 0x7FF
        b_l = (b_l & 0xF800) | (new >> 1) & 0x7FF
        struct.pack_into("<HH", env.full_text, r_offset, b_h, b_l)
    elif reloc_type == "xtensa_l32r":
        l32r = unpack_u24le(env.full_text, r_offset)
        assert l32r & 0xF == 1  # RI16 encoded l32r
        l32r_imm16 = l32r >> 8
        l32r_imm16 = (l32r_imm16 + reloc >> 2) & 0xFFFF
        l32r = l32r & 0xFF | l32r_imm16 << 8
        pack_u24le(env.full_text, r_offset, l32r)
    else:
        assert 0, reloc_type

    # Log information about relocation
    if log_name is None:
        if s_type == "STT_SECTION":
            log_name = s.section.name
        else:
            log_name = s.name
    log(LOG_LEVEL_3, "  {:08x} {} -> {:08x}".format(r_offset, log_name, addr))


def do_relocation_data(env, text_addr, r):
    s = r.sym
    s_type = s.entry["st_info"]["type"]
    r_offset = r["r_offset"] + text_addr
    r_info_type = r["r_info_type"]
    try:
        # only for RELA sections
        r_addend = r["r_addend"]
    except KeyError:
        r_addend = 0

    if (
        env.arch.name == "EM_386"
        and r_info_type == R_386_32
        or env.arch.name == "EM_X86_64"
        and r_info_type == R_X86_64_64
        or env.arch.name == "EM_ARM"
        and r_info_type == R_ARM_ABS32
        or env.arch.name == "EM_XTENSA"
        and r_info_type == R_XTENSA_32
    ):
        # Relocation in data.rel.ro to internal/external symbol
        if env.arch.word_size == 4:
            struct_type = "<I"
        elif env.arch.word_size == 8:
            struct_type = "<Q"
        sec = s.section
        assert r_offset % env.arch.word_size == 0
        addr = sec.addr + s["st_value"] + r_addend
        if s_type == "STT_SECTION":
            log_name = sec.name
        else:
            log_name = s.name
        log(LOG_LEVEL_3, "  {:08x} -> {} {:08x}".format(r_offset, log_name, addr))
        if env.arch.separate_rodata:
            data = env.full_rodata
        else:
            data = env.full_text
        (existing,) = struct.unpack_from(struct_type, data, r_offset)
        if sec.name.startswith((".text", ".rodata", ".data.rel.ro", ".bss")):
            struct.pack_into(struct_type, data, r_offset, existing + addr)
            kind = sec.name
        elif sec.name == ".external.mp_fun_table":
            assert addr == 0
            kind = s.mp_fun_table_offset
        else:
            assert 0, sec.name
        if env.arch.separate_rodata:
            base = ".rodata"
        else:
            base = ".text"
        env.mpy_relocs.append((base, r_offset, kind))

    else:
        # Unknown/unsupported relocation
        assert 0, r_info_type


def load_object_file(env, felf):
    with open(felf, "rb") as f:
        elf = elffile.ELFFile(f)
        env.check_arch(elf["e_machine"])

        # Get symbol table
        symtab = list(elf.get_section_by_name(".symtab").iter_symbols())

        # Load needed sections from ELF file
        sections_shndx = {}  # maps elf shndx to Section object
        for idx, s in enumerate(elf.iter_sections()):
            if s.header.sh_type in ("SHT_PROGBITS", "SHT_NOBITS"):
                if s.data_size == 0:
                    # Ignore empty sections
                    pass
                elif s.name.startswith((".literal", ".text", ".rodata", ".data.rel.ro", ".bss")):
                    sec = Section.from_elfsec(s, felf)
                    sections_shndx[idx] = sec
                    if s.name.startswith(".literal"):
                        env.literal_sections.append(sec)
                    else:
                        env.sections.append(sec)
                elif s.name.startswith(".data"):
                    raise LinkError("{}: {} non-empty".format(felf, s.name))
                else:
                    # Ignore section
                    pass
            elif s.header.sh_type in ("SHT_REL", "SHT_RELA"):
                shndx = s.header.sh_info
                if shndx in sections_shndx:
                    sec = sections_shndx[shndx]
                    sec.reloc_name = s.name
                    sec.reloc = list(s.iter_relocations())
                    for r in sec.reloc:
                        r.sym = symtab[r["r_info_sym"]]

        # Link symbols to their sections, and update known and unresolved symbols
        for sym in symtab:
            sym.filename = felf
            shndx = sym.entry["st_shndx"]
            if shndx in sections_shndx:
                # Symbol with associated section
                sym.section = sections_shndx[shndx]
                if sym["st_info"]["bind"] == "STB_GLOBAL":
                    # Defined global symbol
                    if sym.name in env.known_syms and not sym.name.startswith(
                        "__x86.get_pc_thunk."
                    ):
                        raise LinkError("duplicate symbol: {}".format(sym.name))
                    env.known_syms[sym.name] = sym
            elif sym.entry["st_shndx"] == "SHN_UNDEF" and sym["st_info"]["bind"] == "STB_GLOBAL":
                # Undefined global symbol, needs resolving
                env.unresolved_syms.append(sym)


def link_objects(env, native_qstr_vals_len, native_qstr_objs_len):
    # Build GOT information
    if env.arch.name == "EM_XTENSA":
        build_got_xtensa(env)
    else:
        build_got_generic(env)

    # Creat GOT section
    got_size = len(env.got_entries) * env.arch.word_size
    env.got_section = Section("GOT", bytearray(got_size), env.arch.word_size)
    if env.arch.name == "EM_XTENSA":
        env.sections.insert(0, env.got_section)
    else:
        env.sections.append(env.got_section)

    # Create optional literal section
    if env.arch.name == "EM_XTENSA":
        lit_size = len(env.lit_entries) * env.arch.word_size
        env.lit_section = Section("LIT", bytearray(lit_size), env.arch.word_size)
        env.sections.insert(1, env.lit_section)

    # Create section to contain mp_native_qstr_val_table
    env.qstr_val_section = Section(
        ".text.QSTR_VAL",
        bytearray(native_qstr_vals_len * env.arch.qstr_entry_size),
        env.arch.qstr_entry_size,
    )
    env.sections.append(env.qstr_val_section)

    # Create section to contain mp_native_qstr_obj_table
    env.qstr_obj_section = Section(
        ".text.QSTR_OBJ", bytearray(native_qstr_objs_len * env.arch.word_size), env.arch.word_size
    )
    env.sections.append(env.qstr_obj_section)

    # Resolve unknown symbols
    mp_fun_table_sec = Section(".external.mp_fun_table", b"", 0)
    fun_table = {
        key: 67 + idx
        for idx, key in enumerate(
            [
                "mp_type_type",
                "mp_type_str",
                "mp_type_list",
                "mp_type_dict",
                "mp_type_fun_builtin_0",
                "mp_type_fun_builtin_1",
                "mp_type_fun_builtin_2",
                "mp_type_fun_builtin_3",
                "mp_type_fun_builtin_var",
                "mp_stream_read_obj",
                "mp_stream_readinto_obj",
                "mp_stream_unbuffered_readline_obj",
                "mp_stream_write_obj",
            ]
        )
    }
    for sym in env.unresolved_syms:
        assert sym["st_value"] == 0
        if sym.name == "_GLOBAL_OFFSET_TABLE_":
            pass
        elif sym.name == "mp_fun_table":
            sym.section = Section(".external", b"", 0)
        elif sym.name == "mp_native_qstr_val_table":
            sym.section = env.qstr_val_section
        elif sym.name == "mp_native_qstr_obj_table":
            sym.section = env.qstr_obj_section
        elif sym.name in env.known_syms:
            sym.resolved = env.known_syms[sym.name]
        else:
            if sym.name in fun_table:
                sym.section = mp_fun_table_sec
                sym.mp_fun_table_offset = fun_table[sym.name]
            else:
                raise LinkError("{}: undefined symbol: {}".format(sym.filename, sym.name))

    # Align sections, assign their addresses, and create full_text
    env.full_text = bytearray(env.arch.asm_jump(8))  # dummy, to be filled in later
    env.full_rodata = bytearray(0)
    env.full_bss = bytearray(0)
    for sec in env.sections:
        if env.arch.separate_rodata and sec.name.startswith((".rodata", ".data.rel.ro")):
            data = env.full_rodata
        elif sec.name.startswith(".bss"):
            data = env.full_bss
        else:
            data = env.full_text
        sec.addr = align_to(len(data), sec.alignment)
        data.extend(b"\x00" * (sec.addr - len(data)))
        data.extend(sec.data)

    env.print_sections()

    populate_got(env)
    if env.arch.name == "EM_XTENSA":
        populate_lit(env)

    # Fill in relocations
    for sec in env.sections:
        if not sec.reloc:
            continue
        log(
            LOG_LEVEL_3,
            "{}: {} relocations via {}:".format(sec.filename, sec.name, sec.reloc_name),
        )
        for r in sec.reloc:
            if sec.name.startswith((".text", ".rodata")):
                do_relocation_text(env, sec.addr, r)
            elif sec.name.startswith(".data.rel.ro"):
                do_relocation_data(env, sec.addr, r)
            else:
                assert 0, sec.name


################################################################################
# .mpy output


class MPYOutput:
    def open(self, fname):
        self.f = open(fname, "wb")
        self.prev_base = -1
        self.prev_offset = -1

    def close(self):
        self.f.close()

    def write_bytes(self, buf):
        self.f.write(buf)

    def write_uint(self, val):
        b = bytearray()
        b.insert(0, val & 0x7F)
        val >>= 7
        while val:
            b.insert(0, 0x80 | (val & 0x7F))
            val >>= 7
        self.write_bytes(b)

    def write_qstr(self, s):
        if s in qstrutil.static_qstr_list:
            self.write_bytes(bytes([0, qstrutil.static_qstr_list.index(s) + 1]))
        else:
            s = bytes(s, "ascii")
            self.write_uint(len(s) << 1)
            self.write_bytes(s)

    def write_reloc(self, base, offset, dest, n):
        need_offset = not (base == self.prev_base and offset == self.prev_offset + 1)
        self.prev_offset = offset + n - 1
        if dest <= 2:
            dest = (dest << 1) | (n > 1)
        else:
            assert 6 <= dest <= 127
            assert n == 1
        dest = dest << 1 | need_offset
        assert 0 <= dest <= 0xFE, dest
        self.write_bytes(bytes([dest]))
        if need_offset:
            if base == ".text":
                base = 0
            elif base == ".rodata":
                base = 1
            self.write_uint(offset << 1 | base)
        if n > 1:
            self.write_uint(n)


def build_mpy(env, entry_offset, fmpy, native_qstr_vals, native_qstr_objs):
    # Write jump instruction to start of text
    jump = env.arch.asm_jump(entry_offset)
    env.full_text[: len(jump)] = jump

    log(LOG_LEVEL_1, "arch:         {}".format(env.arch.name))
    log(LOG_LEVEL_1, "text size:    {}".format(len(env.full_text)))
    if len(env.full_rodata):
        log(LOG_LEVEL_1, "rodata size:  {}".format(len(env.full_rodata)))
    log(LOG_LEVEL_1, "bss size:     {}".format(len(env.full_bss)))
    log(LOG_LEVEL_1, "GOT entries:  {}".format(len(env.got_entries)))

    # xxd(env.full_text)

    out = MPYOutput()
    out.open(fmpy)

    # MPY: header
    out.write_bytes(
        bytearray(
            [ord("M"), MPY_VERSION, env.arch.mpy_feature, MP_SMALL_INT_BITS, QSTR_WINDOW_SIZE]
        )
    )

    # MPY: kind/len
    out.write_uint(len(env.full_text) << 2 | (MP_CODE_NATIVE_VIPER - MP_CODE_BYTECODE))

    # MPY: machine code
    out.write_bytes(env.full_text)

    # MPY: n_qstr_link (assumes little endian)
    out.write_uint(len(native_qstr_vals) + len(native_qstr_objs))
    for q in range(len(native_qstr_vals)):
        off = env.qstr_val_section.addr + q * env.arch.qstr_entry_size
        out.write_uint(off << 2)
        out.write_qstr(native_qstr_vals[q])
    for q in range(len(native_qstr_objs)):
        off = env.qstr_obj_section.addr + q * env.arch.word_size
        out.write_uint(off << 2 | 3)
        out.write_qstr(native_qstr_objs[q])

    # MPY: scope_flags
    scope_flags = MP_SCOPE_FLAG_VIPERRELOC
    if len(env.full_rodata):
        scope_flags |= MP_SCOPE_FLAG_VIPERRODATA
    if len(env.full_bss):
        scope_flags |= MP_SCOPE_FLAG_VIPERBSS
    out.write_uint(scope_flags)

    # MPY: n_obj
    out.write_uint(0)

    # MPY: n_raw_code
    out.write_uint(0)

    # MPY: rodata and/or bss
    if len(env.full_rodata):
        rodata_const_table_idx = 1
        out.write_uint(len(env.full_rodata))
        out.write_bytes(env.full_rodata)
    if len(env.full_bss):
        bss_const_table_idx = bool(env.full_rodata) + 1
        out.write_uint(len(env.full_bss))

    # MPY: relocation information
    prev_kind = None
    for base, addr, kind in env.mpy_relocs:
        if isinstance(kind, str) and kind.startswith(".text"):
            kind = 0
        elif kind in (".rodata", ".data.rel.ro"):
            if env.arch.separate_rodata:
                kind = rodata_const_table_idx
            else:
                kind = 0
        elif isinstance(kind, str) and kind.startswith(".bss"):
            kind = bss_const_table_idx
        elif kind == "mp_fun_table":
            kind = 6
        else:
            kind = 7 + kind
        assert addr % env.arch.word_size == 0, addr
        offset = addr // env.arch.word_size
        if kind == prev_kind and base == prev_base and offset == prev_offset + 1:
            prev_n += 1
            prev_offset += 1
        else:
            if prev_kind is not None:
                out.write_reloc(prev_base, prev_offset - prev_n + 1, prev_kind, prev_n)
            prev_kind = kind
            prev_base = base
            prev_offset = offset
            prev_n = 1
    if prev_kind is not None:
        out.write_reloc(prev_base, prev_offset - prev_n + 1, prev_kind, prev_n)

    # MPY: sentinel for end of relocations
    out.write_bytes(b"\xff")

    out.close()


################################################################################
# main


def do_preprocess(args):
    if args.output is None:
        assert args.files[0].endswith(".c")
        args.output = args.files[0][:-1] + "config.h"
    static_qstrs, qstr_vals, qstr_objs = extract_qstrs(args.files)
    with open(args.output, "w") as f:
        print(
            "#include <stdint.h>\n"
            "typedef uintptr_t mp_uint_t;\n"
            "typedef intptr_t mp_int_t;\n"
            "typedef uintptr_t mp_off_t;",
            file=f,
        )
        for i, q in enumerate(static_qstrs):
            print("#define %s (%u)" % (q, i + 1), file=f)
        for i, q in enumerate(sorted(qstr_vals)):
            print("#define %s (mp_native_qstr_val_table[%d])" % (q, i), file=f)
        for i, q in enumerate(sorted(qstr_objs)):
            print(
                "#define MP_OBJ_NEW_QSTR_%s ((mp_obj_t)mp_native_qstr_obj_table[%d])" % (q, i),
                file=f,
            )
        if args.arch == "xtensawin":
            qstr_type = "uint32_t"  # esp32 can only read 32-bit values from IRAM
        else:
            qstr_type = "uint16_t"
        print("extern const {} mp_native_qstr_val_table[];".format(qstr_type), file=f)
        print("extern const mp_uint_t mp_native_qstr_obj_table[];", file=f)


def do_link(args):
    if args.output is None:
        assert args.files[0].endswith(".o")
        args.output = args.files[0][:-1] + "mpy"
    native_qstr_vals = []
    native_qstr_objs = []
    if args.qstrs is not None:
        with open(args.qstrs) as f:
            for l in f:
                m = re.match(r"#define MP_QSTR_([A-Za-z0-9_]*) \(mp_native_", l)
                if m:
                    native_qstr_vals.append(m.group(1))
                else:
                    m = re.match(r"#define MP_OBJ_NEW_QSTR_MP_QSTR_([A-Za-z0-9_]*)", l)
                    if m:
                        native_qstr_objs.append(m.group(1))
    log(LOG_LEVEL_2, "qstr vals: " + ", ".join(native_qstr_vals))
    log(LOG_LEVEL_2, "qstr objs: " + ", ".join(native_qstr_objs))
    env = LinkEnv(args.arch)
    try:
        for file in args.files:
            load_object_file(env, file)
        link_objects(env, len(native_qstr_vals), len(native_qstr_objs))
        build_mpy(env, env.find_addr("mpy_init"), args.output, native_qstr_vals, native_qstr_objs)
    except LinkError as er:
        print("LinkError:", er.args[0])
        sys.exit(1)


def main():
    import argparse

    cmd_parser = argparse.ArgumentParser(description="Run scripts on the pyboard.")
    cmd_parser.add_argument(
        "--verbose", "-v", action="count", default=1, help="increase verbosity"
    )
    cmd_parser.add_argument("--arch", default="x64", help="architecture")
    cmd_parser.add_argument("--preprocess", action="store_true", help="preprocess source files")
    cmd_parser.add_argument("--qstrs", default=None, help="file defining additional qstrs")
    cmd_parser.add_argument(
        "--output", "-o", default=None, help="output .mpy file (default to input with .o->.mpy)"
    )
    cmd_parser.add_argument("files", nargs="+", help="input files")
    args = cmd_parser.parse_args()

    global log_level
    log_level = args.verbose

    if args.preprocess:
        do_preprocess(args)
    else:
        do_link(args)


if __name__ == "__main__":
    main()
