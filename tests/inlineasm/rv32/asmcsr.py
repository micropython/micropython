# test csr instructions

# CSR 0x340 is `mscratch`.  This test suite is only safe to run on a system
# where it is known that there is no other code that can read from or write
# to that register.  The qemu port is one such system, as the CSR is only
# accessed when a machine exception occurs, and at that point it doesn't matter
# anymore whether these tests are running or not.


@micropython.asm_rv32
def csr():
    li(a0, 0)
    csrrw(zero, zero, 0x340)  # All zeroes
    csrrs(a1, zero, 0x340)  # Read zeroes
    c_bnez(a1, end)
    addi(a0, a0, 1)
    li(a1, 0xA5A5A5A5)
    li(a2, 0x5A5A5A5A)
    csrrs(a2, a1, 0x340)  # Read zeroes, set 0xA5A5A5A5
    c_bnez(a2, end)
    addi(a0, a0, 1)
    csrrs(a3, zero, 0x340)  # Read 0xA5A5A5A5
    bne(a3, a1, end)
    addi(a0, a0, 1)
    li(a2, 0xF0F0F0F0)
    csrrc(zero, a2, 0x340)  # Clear upper half
    csrrs(a3, zero, 0x340)  # Read 0x05050505
    xori(a2, a2, -1)
    and_(a2, a1, a2)
    bne(a2, a3, end)
    addi(a0, a0, 1)
    label(end)


print(csr())


@micropython.asm_rv32
def csri():
    li(a0, 0)
    csrrwi(zero, 0x340, 15)  # Write 0xF
    csrrs(a1, zero, 0x340)  # Read 0xF
    csrrsi(a2, 0x340, 0)  # Read
    bne(a1, a2, end)
    addi(a0, a0, 1)
    csrrci(a2, 0x340, 0)  # Read
    bne(a1, a2, end)
    addi(a0, a0, 1)
    li(a2, 15)
    bne(a1, a2, end)
    addi(a0, a0, 1)
    csrrci(zero, 0x340, 1)  # Clear bit 1
    csrrs(a1, zero, 0x340)  # Read 0xE
    li(a2, 14)
    bne(a1, a2, end)
    addi(a0, a0, 1)
    csrrsi(zero, 0x340, 1)  # Set bit 1
    csrrs(a1, zero, 0x340)  # Read 0xF
    li(a2, 15)
    bne(a1, a2, end)
    addi(a0, a0, 1)
    label(end)


print(csri())
