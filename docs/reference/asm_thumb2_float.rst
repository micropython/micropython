Floating point instructions
===========================

These instructions support the use of the ARM floating point coprocessor
(on platforms such as the Pyboard which are equipped with one). The FPU
has 32 registers known as ``s0-s31`` each of which can hold a single
precision float. Data can be passed between the FPU registers and the
ARM core registers with the ``vmov`` instruction.

Note that MicroPython doesn't support passing floats to
assembler functions, nor can you put a float into ``r0`` and expect a
reasonable result. There are two ways to overcome this. The first is to
use arrays, and the second is to pass and/or return integers and convert
to and from floats in code.

Document conventions
--------------------

Notation: ``Sd, Sm, Sn`` denote FPU registers, ``Rd, Rm, Rn`` denote ARM core
registers. The latter can be any ARM core register although registers
``R13-R15`` are unlikely to be appropriate in this context.

Arithmetic
----------

* vadd(Sd, Sn, Sm) ``Sd = Sn + Sm``
* vsub(Sd, Sn, Sm) ``Sd = Sn - Sm``
* vneg(Sd, Sm) ``Sd = -Sm``
* vmul(Sd, Sn, Sm) ``Sd = Sn * Sm``
* vdiv(Sd, Sn, Sm) ``Sd = Sn / Sm``
* vsqrt(Sd, Sm) ``Sd = sqrt(Sm)``

Registers may be identical: ``vmul(S0, S0, S0)`` will execute ``S0 = S0*S0``

Move between ARM core and FPU registers
---------------------------------------

* vmov(Sd, Rm) ``Sd = Rm``
* vmov(Rd, Sm) ``Rd = Sm``

The FPU has a register known as FPSCR, similar to the ARM core's APSR, which stores condition
codes plus other data. The following instructions provide access to this.

* vmrs(APSR\_nzcv, FPSCR)

Move the floating-point N, Z, C, and V flags to the APSR N, Z, C, and V flags.

This is done after an instruction such as an FPU
comparison to enable the condition codes to be tested by the assembler
code. The following is a more general form of the instruction.

* vmrs(Rd, FPSCR) ``Rd = FPSCR``

Move between FPU register and memory
------------------------------------

* vldr(Sd, [Rn, offset]) ``Sd = [Rn + offset]``
* vstr(Sd, [Rn, offset]) ``[Rn + offset] = Sd``

Where ``[Rn + offset]`` denotes the memory address obtained by adding Rn to the offset. This
is specified in bytes. Since each float value occupies a 32 bit word, when accessing arrays of
floats the offset must always be a multiple of four bytes.

Data comparison
---------------

* vcmp(Sd, Sm)

Compare the values in Sd and Sm and set the FPU N, Z,
C, and V flags. This would normally be followed by ``vmrs(APSR_nzcv, FPSCR)``
to enable the results to be tested.

Convert between integer and float
---------------------------------

* vcvt\_f32\_s32(Sd, Sm) ``Sd = float(Sm)``
* vcvt\_s32\_f32(Sd, Sm) ``Sd = int(Sm)``
