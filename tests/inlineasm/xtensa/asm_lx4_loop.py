# On ESP32 the loop counter shouldn't be too large.  LOOP/LOOPNEZ/LOOPGTZ will
# keep the CPU busy, breaking only for an incoming NMI.  This means the RTOS
# will be get stuck and chances are the internal watchdog will engage as the
# RTOS appears to be stuck as far as the internal task scheduling can see.


@micropython.asm_xtensa
def test_loop(a2):
    movi_n(a3, 0)
    movi_n(a4, 0)
    loop(a2, LOOP_END)
    addi(a3, a3, 1)
    movi_n(a4, 1)
    label(LOOP_END)
    addi(a3, a3, 100)
    add(a3, a3, a4)
    mov(a2, a3)


print(test_loop(10))
print(test_loop(20))
print(test_loop(30))


@micropython.asm_xtensa
def test_loopnez(a2):
    movi_n(a3, 0)
    movi_n(a4, 0)
    loopnez(a2, LOOP_END)
    addi(a3, a3, 1)
    movi_n(a4, 1)
    label(LOOP_END)
    addi(a3, a3, 100)
    add(a3, a3, a4)
    mov(a2, a3)


print(test_loopnez(0))
print(test_loopnez(10))
print(test_loopnez(20))


@micropython.asm_xtensa
def test_loopgtz(a2):
    movi_n(a3, 0)
    movi_n(a4, 0)
    loopgtz(a2, LOOP_END)
    addi(a3, a3, 1)
    movi_n(a4, 1)
    label(LOOP_END)
    addi(a3, a3, 100)
    add(a3, a3, a4)
    mov(a2, a3)


print(test_loopgtz(-1))
print(test_loopgtz(0))
print(test_loopgtz(10))
print(test_loopgtz(20))
