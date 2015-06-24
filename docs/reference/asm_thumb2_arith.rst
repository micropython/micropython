Arithmetic instructions
=======================

Document conventions
--------------------

Notation: ``Rd, Rm, Rn`` denote ARM registers R0-R7. ``immN`` denotes an immediate
value having a width of N bits e.g. ``imm8``, ``imm3``. ``carry`` denotes
the carry condition flag, ``not(carry)`` denotes its complement. In the case of instructions
with more than one register argument, it is permissible for some to be identical. For example
the following will add the contents of R0 to itself, placing the result in R0:

* add(r0, r0, r0)

Arithmetic instructions affect the condition flags except where stated.

Addition
--------

* add(Rdn, imm8) ``Rdn = Rdn + imm8``
* add(Rd, Rn, imm3) ``Rd = Rn + imm3``
* add(Rd, Rn, Rm) ``Rd = Rn +Rm``
* adc(Rd, Rn) ``Rd = Rd + Rn + carry``

Subtraction
-----------

* sub(Rdn, imm8) ``Rdn = Rdn - imm8``
* sub(Rd, Rn, imm3) ``Rd = Rn - imm3``
* sub(Rd, Rn, Rm) ``Rd = Rn - Rm``
* sbc(Rd, Rn) ``Rd = Rd - Rn - not(carry)``

Negation
--------

* neg(Rd, Rn) ``Rd = -Rn``

Multiplication and division
---------------------------

* mul(Rd, Rn) ``Rd = Rd * Rn``

This produces a 32 bit result with overflow lost. The result may be treated as
signed or unsigned according to the definition of the operands.

* sdiv(Rd, Rn, Rm) ``Rd = Rn / Rm``
* udiv(Rd, Rn, Rm) ``Rd = Rn / Rm``

These functions perform signed and unsigned division respectively. Condition flags
are not affected.
