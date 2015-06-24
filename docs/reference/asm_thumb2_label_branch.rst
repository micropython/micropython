Branch instructions
===================

These cause execution to jump to a target location usually specified by a label (see the ``label``
assembler directive). Conditional branches and the ``it`` and ``ite`` instructions test
the Application Program Status Register (APSR) N (negative), Z (zero), C (carry) and V
(overflow) flags to determine whether the branch should be executed.

Most of the exposed assembler instructions (including move operations) set the flags but
there are explicit comparison instructions to enable values to be tested.

Further detail on the meaning of the condition flags is provided in the section
describing comparison functions.

Document conventions
--------------------

Notation: ``Rm`` denotes ARM registers R0-R15. ``LABEL`` denotes a label defined with the
``label()`` assembler directive. ``<condition>`` indicates one of the following condition
specifiers:

* eq Equal to (result was zero)
* ne Not equal
* cs Carry set
* cc Carry clear
* mi Minus (negaive)
* pl Plus (positive)
* vs Overflow set
* vc Overflow clear
* hi > (unsigned comparison)
* ls <= (unsigned comparison)
* ge >= (signed comparison)
* lt < (signed comparison)
* gt > (signed comparison)
* le <= (signed comparison)

Branch to label
---------------

* b(LABEL) Unconditional branch
* beq(LABEL) branch if equal
* bne(LABEL) branch if not equal
* bge(LABEL) branch if greater than or equal
* bgt(LABEL) branch if greater than
* blt(LABEL) branch if less than (<) (signed)
* ble(LABEL) branch if less than or equal to (<=) (signed)
* bcs(LABEL) branch if carry flag is set
* bcc(LABEL) branch if carry flag is clear
* bmi(LABEL) branch if negative
* bpl(LABEL) branch if positive
* bvs(LABEL) branch if overflow flag set
* bvc(LABEL) branch if overflow flag is clear
* bhi(LABEL) branch if higher (unsigned)
* bls(LABEL) branch if lower or equal (unsigned)

Long branches
-------------

The code produced by the branch instructions listed above uses a fixed bit width to specify the
branch destination, which is PC relative. Consequently in long programs where the
branch instruction is remote from its destination the assembler will produce a "branch not in
range" error. This can be overcome with the "wide" variants such as

* beq\_w(LABEL) long branch if equal

Wide branches use 4 bytes to encode the instruction (compared with 2 bytes for standard branch instructions).

Subroutines (functions)
-----------------------

When entering a subroutine the processor stores the return address in register r14, also
known as the link register (lr). Return to the instruction after the subroutine call is
performed by updating the program counter (r15 or pc) from the link register, This
process is handled by the following instructions.

* bl(LABEL)

Transfer execution to the instruction after ``LABEL`` storing the return address in
the link register (r14).

* bx(Rm) Branch to address specified by Rm.

Typically ``bx(lr)`` is issued to return from a subroutine. For nested subroutines the
link register of outer scopes must be saved (usually on the stack) before performing
inner subroutine calls.
