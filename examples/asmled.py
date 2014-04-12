# flash LED #1 using inline assembler
@micropython.asm_thumb
def flash_led(r0):
    movw(r1, (stm.GPIOA + stm.GPIO_BSRRL) & 0xffff)
    movt(r1, ((stm.GPIOA + stm.GPIO_BSRRL) >> 16) & 0x7fff)
    movw(r2, 1 << 13)
    movt(r2, 0)
    movw(r3, 0)
    movt(r3, 1 << 13)

    b(loop_entry)

    label(loop1)

    # turn LED on
    str(r2, r1, 0)

    # delay for a bit
    movw(r4, 5599900 & 0xffff)
    movt(r4, (5599900 >> 16) & 0xffff)
    label(delay_on)
    subs(r4, r4, 1)
    cmp(r4, 0)
    bgt(delay_on)

    # turn LED off
    str(r3, r1, 0)

    # delay for a bit
    movw(r4, 5599900 & 0xffff)
    movt(r4, (5599900 >> 16) & 0xffff)
    label(delay_off)
    subs(r4, r4, 1)
    cmp(r4, 0)
    bgt(delay_off)

    # loop r0 times
    subs(r0, r0, 1)
    label(loop_entry)
    cmp(r0, 0)
    bgt(loop1)

# flash LED #1 using inline assembler
# this version uses the convenience assembler operation 'movwt'
@micropython.asm_thumb
def flash_led_v2(r0):
    movwt(r1, stm.GPIOA + stm.GPIO_BSRRL)
    movwt(r2, 1 << 13)
    movwt(r3, 1 << (16 + 13))

    b(loop_entry)

    label(loop1)

    # turn LED on
    str(r2, r1, 0)

    # delay for a bit
    movwt(r4, 5599900)
    label(delay_on)
    subs(r4, r4, 1)
    cmp(r4, 0)
    bgt(delay_on)

    # turn LED off
    str(r3, r1, 0)

    # delay for a bit
    movwt(r4, 5599900)
    label(delay_off)
    subs(r4, r4, 1)
    cmp(r4, 0)
    bgt(delay_off)

    # loop r0 times
    subs(r0, r0, 1)
    label(loop_entry)
    cmp(r0, 0)
    bgt(loop1)

flash_led_v2(5)
