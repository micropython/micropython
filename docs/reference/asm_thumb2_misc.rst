Miscellaneous instructions
==========================

* nop() ``pass`` no operation.
* wfi() Suspend execution in a low power state until an interrupt occurs.
* cpsid(flags) set the Priority Mask Register - disable interrupts.
* cpsie(flags) clear the Priority Mask Register - enable interrupts.

Currently the ``cpsie()`` and ``cpsid()`` functions are partially implemented.
They require but ignore the flags argument and serve as a means of enabling and disabling interrupts.
