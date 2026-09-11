# test bkpt instruction compiles in inline assembler
# bkpt generates a breakpoint (SIGTRAP) so we only test compilation,
# not execution


@micropython.asm_aarch64
def asm_bkpt(x0) -> int:
    mov(x0, 42)
    bkpt()


print("bkpt compiled ok")
