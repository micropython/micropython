Register move instructions
==========================

Document conventions
--------------------

Notation: ``Rd, Rn`` denote ARM registers R0-R15. ``immN`` denotes an immediate
value having a width of N bits. These instructions affect the condition flags.

Register moves
--------------

Where immediate values are used, these are zero-extended to 32 bits. Thus
``mov(R0, 0xff)`` will set R0 to 255.

* mov(Rd, imm8) ``Rd = imm8``
* mov(Rd, Rn) ``Rd = Rn``
* movw(Rd, imm16) ``Rd = imm16``
* movt(Rd, imm16) ``Rd = (Rd & 0xffff) | (imm16 << 16)``

movt writes an immediate value to the top halfword of the destination register.
It does not affect the contents of the bottom halfword.

* movwt(Rd, imm30) ``Rd = imm30``

movwt is a pseudo-instruction: the MicroPython assembler emits a ``movw`` and a ``movt``
to move a zero extended 30 bit value into Rd. Where the full 32 bits are required a
workround is to use the movw and movt operations.
