Miscellaneous instructions
==========================

* nop() ``pass`` no operation.
* wfi() Suspend execution in a low power state until an interrupt occurs.
* cpsid(flags) set the Priority Mask Register - disable interrupts.
* cpsie(flags) clear the Priority Mask Register - enable interrupts.
* mrs(Rd, special_reg) ``Rd = special_reg`` copy a special register to a general register. The special register
  may be IPSR (Interrupt Status Register) or BASEPRI (Base Priority Register). The IPSR provides a means of determining
  the exception number of an interrupt being processed. It contains zero if no interrupt is being processed.

Currently the ``cpsie()`` and ``cpsid()`` functions are partially implemented.
They require but ignore the flags argument and serve as a means of enabling and disabling interrupts.
