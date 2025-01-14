TEMPLATE3 = """
@micropython.asm_rv32
def f():
    {}({}, {}, {})
"""

TEMPLATE2 = """
@micropython.asm_rv32
def f():
    {}({}, {})
"""

TEMPLATE1 = """
@micropython.asm_rv32
def f():
    {}({})
"""


REGISTERS = [
    "zero",
    "s0",
    "s1",
    "s2",
    "s3",
    "s4",
    "s5",
    "s6",
    "s7",
    "s8",
    "s9",
    "s10",
    "s11",
    "a0",
    "a1",
    "a2",
    "a3",
    "a4",
    "a5",
    "a6",
    "a7",
    "tp",
    "gp",
    "sp",
    "ra",
    "t0",
    "t1",
    "t2",
    "t3",
    "t4",
    "t5",
    "t6",
    "x0",
    "x1",
    "x2",
    "x3",
    "x4",
    "x5",
    "x6",
    "x7",
    "x8",
    "x9",
    "x10",
    "x11",
    "x12",
    "x13",
    "x14",
    "x15",
    "x16",
    "x17",
    "x18",
    "x19",
    "x20",
    "x21",
    "x22",
    "x23",
    "x24",
    "x25",
    "x26",
    "x27",
    "x28",
    "x29",
    "x30",
    "x31",
]


def harness(opcode, fragment, tag):
    try:
        exec(fragment)
    except SyntaxError:
        print(tag, opcode)


for opcode in ("slli", "srli", "srai"):
    harness(opcode, TEMPLATE3.format(opcode, "a0", "a0", -1), "-")
    harness(opcode, TEMPLATE3.format(opcode, "a0", "a0", 33), "+")

for opcode in ("c_slli", "c_srli", "c_srai"):
    harness(opcode, TEMPLATE2.format(opcode, "a0", -1), "-")
    harness(opcode, TEMPLATE2.format(opcode, "a0", 33), "+")

harness("c_slli", TEMPLATE2.format("c_slli", "zero", 0), "0")
harness("c_slli", TEMPLATE2.format("c_slli", "x0", 0), "0")

for opcode in ("c_srli", "c_srai"):
    for register in REGISTERS:
        harness(opcode, TEMPLATE2.format(opcode, register, 0), register)

for opcode in ("c_mv", "c_add"):
    harness(opcode, TEMPLATE2.format(opcode, "a0", "zero"), "0l")
    harness(opcode, TEMPLATE2.format(opcode, "zero", "a0"), "0r")
    harness(opcode, TEMPLATE2.format(opcode, "zero", "zero"), "0b")

harness("c_jr", TEMPLATE1.format("c_jr", "zero"), "0")

for opcode in ("addi", "andi", "ori", "slti", "sltiu", "xori"):
    harness(opcode, TEMPLATE3.format(opcode, "a0", "a0", 0x7FF), ">=s")
    harness(opcode, TEMPLATE3.format(opcode, "a0", "a0", 0x800), ">s")
    harness(opcode, TEMPLATE3.format(opcode, "a0", "a0", -2048), "<=s")
    harness(opcode, TEMPLATE3.format(opcode, "a0", "a0", -2049), "<s")

for opcode in ("lb", "lbu", "lh", "lhu", "lw", "sb", "sh", "sw"):
    TEMPLATE = """
@micropython.asm_rv32
def f():
    {}(a0, {}(a0))
"""
    harness(opcode, TEMPLATE.format(opcode, 0x7FF), ">=s")
    harness(opcode, TEMPLATE.format(opcode, 0x800), ">s")
    harness(opcode, TEMPLATE.format(opcode, -2048), "<=s")
    harness(opcode, TEMPLATE.format(opcode, -2049), "<s")

harness("c_addi", TEMPLATE2.format("c_andi", "zero", 0), "00")
harness("c_addi", TEMPLATE2.format("c_andi", "zero", 512), ">0")
harness("c_addi", TEMPLATE2.format("c_andi", "zero", -512), "<0")
harness("c_addi", TEMPLATE2.format("c_andi", "s0", 0), "s0")
harness("c_addi", TEMPLATE2.format("c_andi", "s0", -100), "<s")
harness("c_addi", TEMPLATE2.format("c_andi", "s0", 100), ">s")

harness("c_andi", TEMPLATE2.format("c_andi", "zero", 0), "00")
harness("c_andi", TEMPLATE2.format("c_andi", "zero", 512), ">0")
harness("c_andi", TEMPLATE2.format("c_andi", "zero", -512), "<0")
harness("c_andi", TEMPLATE2.format("c_andi", "s0", 0), "s0")
harness("c_andi", TEMPLATE2.format("c_andi", "s0", -100), "<s")
harness("c_andi", TEMPLATE2.format("c_andi", "s0", 100), ">s")

C_REGISTERS = (
    "a0",
    "a1",
    "a2",
    "a3",
    "a4",
    "a5",
    "s0",
    "s1",
    "x8",
    "x9",
    "x10",
    "x11",
    "x12",
    "x13",
    "x14",
    "x15",
)

for opcode in ("c_and", "c_or", "c_xor"):
    for source in REGISTERS:
        for destination in REGISTERS:
            if source in C_REGISTERS and destination in C_REGISTERS:
                try:
                    exec(
                        """
@micropython.asm_rv32
def f():
    {}({}, {})
""".format(opcode, source, destination)
                    )
                except SyntaxError:
                    print(source, destination, opcode)
            else:
                try:
                    exec(
                        """
@micropython.asm_rv32
def f():
    {}({}, {})
""".format(opcode, source, destination)
                    )
                    print(source, destination, opcode)
                except SyntaxError:
                    pass
    print(opcode)

for opcode in ("c_lw", "c_sw"):
    TEMPLATE = """
@micropython.asm_rv32
def f():
    {}(a0, {}(a0))
"""
    harness(opcode, TEMPLATE.format(opcode, 60), ">=s")
    harness(opcode, TEMPLATE.format(opcode, 61), ">s")
    harness(opcode, TEMPLATE.format(opcode, -60), "<=s")
    harness(opcode, TEMPLATE.format(opcode, -61), "<s")
