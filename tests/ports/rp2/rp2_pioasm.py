import rp2

@rp2.asm_pio()
def ops1():
    label("wait")
    wait(0, gpio, 9)
    wait(1, pin, 23)
    wait(1, irq, 1)
    jmp("out")

    label("in_")
    in_(pins, 18)
    in_(x, 25)
    in_(y, 31)
    in_(null, 2)
    in_(isr, 26)
    in_(osr, 30)
    jmp(not_x, "push")
    jmp(x_dec, "wait")

    label("out")
    out(pins, 24)
    out(x, 11)
    out(y, 25)
    out(null, 17)
    out(isr, 25)
    out(osr, 1)
    jmp("in_")

    label("push")
    push()
    push(block)
    push(noblock)
    push(iffull)
    push(iffull, block)
    push(iffull, noblock)
    jmp(not_y, "wait")
    jmp(y_dec, "out")
print("ops1:", ops1)

@rp2.asm_pio()
def ops2():
    label("pull")
    pull()
    pull(block)
    pull(noblock)
    pull(ifempty)
    pull(ifempty, block)
    pull(ifempty, noblock)
    jmp(x_not_y, "irq")

    wrap_target()
    label("mov")
    mov(pins, y)
    mov(x, reverse(exec))
    mov(y, invert(pc))
    mov(exec, isr)
    mov(pc, reverse(osr))
    mov(isr, invert(pins))
    mov(osr, x)
    nop()
    jmp(not_osre, "set")

    label("irq")
    irq(3)
    irq(rel(0))
    irq(block, 7)
    irq(clear, rel(1))
    jmp(pin, "mov")

    wrap()
    label("set")
    set(pins, 9)
    set(x, 2)
    set(y, 30)
    set(pindirs, 11)
print("ops2:", ops2)

@rp2.asm_pio(sideset_init=[rp2.PIO.OUT_LOW] * 3)
def side1():
    label("side")
    out(x, 23).side(3).delay(3)
    jmp(y_dec, "side").side(2)
    mov(osr, reverse(x)).side(1)[1]
print("side1:", side1)

try:
    @rp2.asm_pio(sideset_init=[rp2.PIO.OUT_LOW] * 1)
    def side2():
        label("side")
        out(x, 23).side(3).delay(3)
        jmp(y_dec, "side").side(2)
        mov(osr, reverse(x)).side(1)[1]
    print("side2:", side2)
except rp2.PIOASMError:
    print("side2: overflow correctly detected")

try:
    @rp2.asm_pio(sideset_init=[rp2.PIO.OUT_LOW] * 3)
    def side3():
        label("side")
        out(x, 23).side(3).delay(3)
        jmp(y_dec, "side")
        mov(osr, reverse(x)).side(1)[1]
    print("side3:", side3)
except rp2.PIOASMError:
    print("side3: overflow correctly detected")

@rp2.asm_pio(
    out_init=[rp2.PIO.OUT_HIGH] * 7,
    set_init=[rp2.PIO.OUT_HIGH, rp2.PIO.OUT_LOW] * 2,
)
def init1():
    nop()
print("init1:", init1)

@rp2.asm_pio(
    out_init=[rp2.PIO.OUT_LOW, rp2.PIO.OUT_HIGH] * 3,
    set_init=[rp2.PIO.OUT_LOW] * 5,
)
def init2():
    nop()
print("init2:", init2)

@rp2.asm_pio(side_pindir=True)
def pindir1():
    nop()
print("pindir1:", pindir1)

@rp2.asm_pio(side_pindir=False)
def pindir2():
    nop()
print("pindir2:", pindir2)

@rp2.asm_pio(autopush=True, push_thresh=23)
def autopush1():
    nop()
print("autopush1:", autopush1)

@rp2.asm_pio(autopush=False, push_thresh=5)
def autopush2():
    nop()
print("autopush2:", autopush2)

@rp2.asm_pio(autopull=True, pull_thresh=13)
def autopull1():
    nop()
print("autopull1:", autopull1)

@rp2.asm_pio(autopull=False, pull_thresh=31)
def autopull2():
    nop()
print("autopull2:", autopull2)

@rp2.asm_pio(in_shiftdir=rp2.PIO.SHIFT_LEFT, out_shiftdir=rp2.PIO.SHIFT_RIGHT)
def shiftdir1():
    nop()
print("shiftdir1:", shiftdir1)

@rp2.asm_pio(in_shiftdir=rp2.PIO.SHIFT_RIGHT, out_shiftdir=rp2.PIO.SHIFT_LEFT)
def shiftdir2():
    nop()
print("shiftdir2:", shiftdir2)

@rp2.asm_pio(fifo_join=rp2.PIO.JOIN_RX)
def join1():
    nop()
print("join1:", join1)

@rp2.asm_pio(fifo_join=rp2.PIO.JOIN_TX)
def join2():
    nop()
print("join2:", join2)

@rp2.asm_pio(fifo_join=rp2.PIO.JOIN_NONE)
def join3():
    nop()
print("join3:", join3)
