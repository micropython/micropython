.. _asm_aarch64:

Inline assembler for AArch64
============================

This document assumes some familiarity with assembly language programming
and with the ARM 64-bit (AArch64/ARMv8-A) instruction set.  For a detailed
description of the instruction set consult the *Arm Architecture Reference
Manual for A-profile architecture*.

The inline assembler is enabled on AArch64 targets (for example the unix
port running on a 64-bit ARM system, or the ``VIRT_AARCH64`` boards of the
``ports/qemu`` port) and is used via the ``@micropython.asm_aarch64``
decorator::

    @micropython.asm_aarch64
    def add1(x0) -> int:
        add(x0, x0, 1)

    print(add1(1))  # prints 2

Instructions are written as Python function calls; operands are passed as
arguments.  The syntax tries to be as close as possible to that defined in
the ARM manual, converted to Python function calls.  Because ``and`` is a
Python keyword, the AND instruction is spelled ``and_``.

Registers and calling convention
--------------------------------

All 31 general purpose registers are available as ``x0``-``x30``, plus
``sp`` and ``lr`` (an alias for ``x30``).  All instructions operate on 64-bit
values (the ``x`` registers); the 32-bit ``w`` registers are not accessible
from the inline assembler.

A function may take up to four parameters, which must be named ``x0``,
``x1``, ``x2``, ``x3`` in sequence; the arguments passed by the caller are
available in those registers.  Unless another return type is annotated, the
value in ``x0`` when control reaches the end of the function body is treated
as a signed integer result.

``sp`` is only accepted as an operand of ``push`` and ``pop``; every other
instruction expects ``x0``-``x30`` (or ``lr``).  Adjust the stack with those
two operations rather than with ``add``/``sub``.

.. warning::

   Do not use ``ret()`` to return from the assembly function itself.  Leave
   the value in ``x0`` and let control fall off the end of the body, so that
   the automatically generated epilogue runs.  A ``ret()`` at the top level
   skips that epilogue, which leaves the stack pointer 96 bytes too low and
   ``x19``-``x28`` unrestored, corrupting the caller.  ``ret()`` is only for
   returning from a subroutine reached with ``bl()``, as in the example
   below.

The function wrapper saves and restores all callee-saved registers
(``x19``-``x28``) as well as the frame pointer and link register, so user
assembly code may freely use any register.  Note that the compiler may use
``x9``/``x10`` as scratch registers when materialising a large immediate
for ``and_``/``eor``/``orr`` that cannot be encoded directly (see below);
this never affects the operands of the instruction being assembled.

Instructions
------------

The following subset of the AArch64 instruction set is supported.

Arithmetic and logical
~~~~~~~~~~~~~~~~~~~~~~

* ``add(Rd, Rn, Rm)`` / ``add(Rd, Rn, imm)`` — ``Rd = Rn + Rm`` or ``Rd = Rn + imm``
* ``sub(Rd, Rn, Rm)`` / ``sub(Rd, Rn, imm)`` — ``Rd = Rn - Rm`` or ``Rd = Rn - imm``
* ``mul(Rd, Rn, Rm)`` — ``Rd = Rn * Rm`` (signed and unsigned, low 64 bits)
* ``and_(Rd, Rn, Rm)`` / ``and_(Rd, Rn, imm)`` — bitwise AND
* ``orr(Rd, Rn, Rm)`` / ``orr(Rd, Rn, imm)`` — bitwise OR
* ``eor(Rd, Rn, Rm)`` / ``eor(Rd, Rn, imm)`` — bitwise exclusive OR
* ``mvn(Rd, Rm)`` — bitwise NOT
* ``cmp(Rn, Rm)`` / ``cmp(Rn, imm)`` — compare (sets the condition flags)
* ``mov(Rd, Rn)`` / ``mov(Rd, imm)`` — move register or 64-bit immediate

For ``add``/``sub``/``cmp`` the immediate must be a 12-bit value
(``0``-``0xfff``), optionally shifted left by 12 bits (i.e. a multiple of
4096 up to ``0xfff000``).

For ``and_``/``eor``/``orr`` any 64-bit immediate is accepted.  Immediates
that match the ARM "bitmask immediate" encoding (a rotated run of one bits
replicated across the register, e.g. ``0xff``, ``0xf0``, ``0xffffffff``,
``~0xf``) are emitted as a single instruction; other values are
materialised in a scratch register first.

For ``mov`` any 64-bit immediate is accepted; the shortest ``MOVZ``/``MOVN``/
``MOVK`` sequence is generated.

Shifts
~~~~~~

* ``lsl(Rd, Rn, Rm)`` / ``lsl(Rd, Rn, imm)`` — logical shift left
* ``lsr(Rd, Rn, Rm)`` / ``lsr(Rd, Rn, imm)`` — logical shift right
* ``asr(Rd, Rn, Rm)`` / ``asr(Rd, Rn, imm)`` — arithmetic shift right

The immediate shift amount must be in the range 0-63.

Load and store
~~~~~~~~~~~~~~

* ``ldr(Rt, [Rn, off])`` — load 64-bit word; ``off`` must be a multiple of 8, in 0-32760
* ``ldrh(Rt, [Rn, off])`` — load 16-bit halfword (zero extend); ``off`` must be a multiple of 2, in 0-8190
* ``ldrb(Rt, [Rn, off])`` — load byte (zero extend); ``off`` in 0-4095
* ``str(Rt, [Rn, off])`` / ``strh(Rt, [Rn, off])`` / ``strb(Rt, [Rn, off])`` — the store equivalents

The offset is a constant in bytes; unaligned offsets for ``ldr``/``str``/
``ldrh``/``strh`` are a compile-time error.

Branches and labels
~~~~~~~~~~~~~~~~~~~

* ``label(NAME)`` — define a label
* ``b(label)`` — unconditional branch
* ``bl(label)`` — branch with link (call); return with ``ret(lr)``
* ``b<cc>(label)`` — conditional branch, where ``<cc>`` is one of:
  ``eq``, ``ne``, ``cs``, ``cc``, ``mi``, ``pl``, ``vs``, ``vc``, ``hi``,
  ``ls``, ``ge``, ``lt``, ``gt``, ``le``.  ``bhs`` and ``blo`` are accepted as
  aliases for ``bcs`` and ``bcc``.  The condition code is written directly
  after the ``b`` with no separator, because ``b.eq`` is not a valid Python
  identifier.
* ``ret(Rn)`` — return to the address in ``Rn`` (usually ``lr``)
* ``nop()``, ``wfi()``, ``bkpt()`` — no operation, wait for interrupt,
  breakpoint (``BRK #0``)

``b`` and ``bl`` have a native range of ±128MB and are always a single
instruction.  ``b<cc>`` has a native range of only ±1MB, so a conditional
branch that is forwards, or backwards beyond ±1MB, is relaxed automatically to
a two-instruction sequence (an inverted ``b<cc>`` over an unconditional ``b``),
which also gives it a ±128MB range.  Because the relaxation is decided from
the branch direction rather than from whether the label is known yet, the
generated code has the same size in every compiler pass.

Stack operations
~~~~~~~~~~~~~~~~

* ``push({Ra, Rb, ...})`` — store the listed registers on the stack
  (decrementing ``sp``; an odd number of registers is padded with a dummy
  slot to keep ``sp`` 16-byte aligned)
* ``pop({Ra, Rb, ...})`` — the inverse of ``push``

Example::

    @micropython.asm_aarch64
    def call_helper(x0) -> int:
        push({lr})
        bl(double)
        pop({lr})
        ret(lr)
        label(double)
        add(x0, x0, x0)
        ret(lr)

Limitations
-----------

- Register-offset addressing (e.g. ``ldr(x0, [x1, x2])``) is not
  supported; only constant offsets.  Pre-indexed (``[x1, #8]!``) and
  post-indexed (``[x1], #8``) forms cannot be written in Python at all.
- ``push`` and ``pop`` round an odd number of registers up to an even number of
  stack slots, to keep ``sp`` 16-byte aligned.  The padding slot is written
  with, and read back into, the zero register, so it never disturbs ``sp``.
  Matching ``push``/``pop`` lists therefore keep the stack balanced as long as
  they round to the same number of slots.
- Floating point (SIMD/FP) registers and instructions are not supported.
- There is currently no ``.mpy`` architecture ID for AArch64, so inline
  assembler functions cannot be frozen into ``.mpy`` files or loaded from
  them; they are compiled at runtime only.
