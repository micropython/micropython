Comparison instructions
=======================

These perform an arithmetic or logical instruction on two arguments, discarding the result
but setting the condition flags. Typically these are used to test data values without changing
them prior to executing a conditional branch.

Document conventions
--------------------

Notation: ``Rd, Rm, Rn`` denote ARM registers R0-R7. ``imm8`` denotes an immediate
value having a width of 8 bits.

The Application Program Status Register (APSR)
----------------------------------------------

This contains four bits which are tested by the conditional branch instructions. Typically a
conditional branch will test multiple bits, for example ``bge(LABEL)``. The meaning of
condition codes can depend on whether the operands of an arithmetic instruction are viewed as
signed or unsigned integers. Thus ``bhi(LABEL)`` assumes unsigned numbers were processed while
``bgt(LABEL)`` assumes signed operands.

APSR Bits
---------

* Z (zero)

This is set if the result of an operation is zero or the operands of a comparison are equal.

* N (negative)

Set if the result is negative.

* C (carry)

An addition sets the carry flag when the result overflows out of the MSB, for example adding
0x80000000 and 0x80000000. By the nature of two's complement arithmetic this behaviour is reversed
on subtraction, with a borrow indicated by the carry bit being clear. Thus 0x10 - 0x01 is executed
as 0x10 + 0xffffffff which will set the carry bit.

* V (overflow)

The overflow flag is set if the result, viewed as a two's compliment number, has the "wrong" sign
in relation to the operands. For example adding 1 to 0x7fffffff will set the overflow bit because
the result (0x8000000), viewed as a two's complement integer, is negative. Note that in this instance
the carry bit is not set.

Comparison instructions
-----------------------

These set the APSR (Application Program Status Register) N (negative), Z (zero), C (carry) and V
(overflow) flags.

* cmp(Rn, imm8) ``Rn - imm8``
* cmp(Rn, Rm) ``Rn - Rm``
* cmn(Rn, Rm) ``Rn + Rm``
* tst(Rn, Rm) ``Rn & Rm``

Conditional execution
---------------------

The ``it`` and ``ite`` instructions provide a means of conditionally executing from one to four subsequent
instructions without the need for a label.

* it(<condition>) If then

Execute the next instruction if <condition> is true:

::

    cmp(r0, r1)
    it(eq)
    mov(r0, 100) # runs if r0 == r1
    # execution continues here

* ite(<condition>) If then else

If <condition> is true, execute the next instruction, otherwise execute the
subsequent one. Thus:

::

    cmp(r0, r1)
    ite(eq)
    mov(r0, 100) # runs if r0 == r1
    mov(r0, 200) # runs if r0 != r1
    # execution continues here

This may be extended to control the execution of up to four subsequent instructions: it[x[y[z]]]
where x,y,z=t/e; e.g. itt, itee, itete, ittte, itttt, iteee, etc.
