import mpy_coverage

mpy_coverage.run_one_arch("rv32imc", "rv32")

print("Expected to compile")
mpy_coverage.run_extra_cases(
    "rv32imc",
    [
        "@micropython.asm_rv32\ndef f(): ebreak()",
    ],
)

# Test "argument out of range" specifically for label targets
nl = "\n"
beqz_test = f"""
@micropython.asm_rv32
def f():
 c_beqz(a0, l)
{nl.join(" ecall()" for _ in range(64))}
 label(l)
 """

print("Expected to fail")
mpy_coverage.run_extra_cases(
    "rv32imc",
    [
        "@micropython.asm_rv32\ndef f(a0, a1, a2, a3, a4): ac_li(a0, 1)",
        "@micropython.asm_rv32\ndef f(a0, a4): ac_li(a0, 1)",
        "@micropython.asm_rv32\ndef f(a0): csrrc()",
        "@micropython.asm_rv32\ndef f(a0): csrrc(s0, s0, s0)",
        "@micropython.asm_rv32\ndef f(a0): csrrc(s0, s0, 0xffffff)",
        "@micropython.asm_rv32\ndef f(a0): csrrc(s0, 0, s0)",
        "@micropython.asm_rv32\ndef f(a0): moo()",
        "@micropython.asm_rv32\ndef f(a0): beq(a0,a0,0)",
        "@micropython.asm_rv32\ndef f(a0): beq(a0,a0,3.14)",
        "@micropython.asm_rv32\ndef f(a0): beq(a0,a0,y)",
        "@micropython.asm_rv32\ndef f(): lw(a3, 99999(a3))",
        "@micropython.asm_rv32\ndef f(): lw(a3, -99999(a3))",
        "@micropython.asm_rv32\ndef f(): lw(a3, +99999(a3))",
        "@micropython.asm_rv32\ndef f(): lw(a3, (a3))",
        "@micropython.asm_rv32\ndef f(): lw()",
        "@micropython.asm_rv32\ndef f(): a=di(a2, a2, -1)",
        "@micropython.asm_rv32\ndef f(ax): beq(ax, ax, 1)",
        "@micropython.asm_rv32\ndef f(): c_lui(zero, 0)",
        "@micropython.asm_rv32\ndef f(): c_lui(a0, 0)",
        "@micropython.asm_rv32\ndef f(): csrrci(a0, 63, 63)",
        beqz_test,
    ],
)
