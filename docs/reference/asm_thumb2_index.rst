.. _asm_thumb2_index:

Inline Assembler for Thumb2 architectures
=========================================

This document assumes some familiarity with assembly language programming and should be read after studying
the :ref:`tutorial <pyboard_tutorial_assembler>`. For a detailed description of the instruction set consult the
Architecture Reference Manual detailed below.
The inline assembler supports a subset of the ARM Thumb-2 instruction set described here. The syntax tries
to be as close as possible to that defined in the above ARM manual, converted to Python function calls.

Instructions operate on 32 bit signed integer data except where stated otherwise. Most supported instructions
operate on registers ``R0-R7`` only: where ``R8-R15`` are supported this is stated. Registers ``R8-R12`` must be
restored to their initial value before return from a function. Registers ``R13-R15`` constitute the Link Register,
Stack Pointer and Program Counter respectively.

Document conventions
--------------------

Where possible the behaviour of each instruction is described in Python, for example

* add(Rd, Rn, Rm) ``Rd = Rn + Rm``

This enables the effect of instructions to be demonstrated in Python. In certain case this is impossible
because Python doesn't support concepts such as indirection. The pseudocode employed in such cases is
described on the relevant page.

Instruction Categories
----------------------

The following sections details the subset of the ARM Thumb-2 instruction set supported by MicroPython.

.. toctree::
   :maxdepth: 1
   :numbered:

   asm_thumb2_mov.rst
   asm_thumb2_ldr.rst
   asm_thumb2_str.rst
   asm_thumb2_logical_bit.rst
   asm_thumb2_arith.rst
   asm_thumb2_compare.rst
   asm_thumb2_label_branch.rst
   asm_thumb2_stack.rst
   asm_thumb2_misc.rst
   asm_thumb2_float.rst
   asm_thumb2_directives.rst

Usage examples
--------------

These sections provide further code examples and hints on the use of the assembler.

.. toctree::
   :maxdepth: 1
   :numbered:

   asm_thumb2_hints_tips.rst

References
----------

-  :ref:`Assembler Tutorial <pyboard_tutorial_assembler>`
-  `Wiki hints and tips
   <http://wiki.micropython.org/platforms/boards/pyboard/assembler>`__
-  `uPy Inline Assembler source-code,
   emitinlinethumb.c <https://github.com/micropython/micropython/blob/master/py/emitinlinethumb.c>`__
-  `ARM Thumb2 Instruction Set Quick Reference
   Card <http://infocenter.arm.com/help/topic/com.arm.doc.qrc0001l/QRC0001_UAL.pdf>`__
-  `RM0090 Reference
   Manual <http://www.google.ae/url?sa=t&rct=j&q=&esrc=s&source=web&cd=1&cad=rja&uact=8&sqi=2&ved=0CBoQFjAA&url=http%3A%2F%2Fwww.st.com%2Fst-web-ui%2Fstatic%2Factive%2Fen%2Fresource%2Ftechnical%2Fdocument%2Freference_manual%2FDM00031020.pdf&ei=G0rSU66xFeuW0QWYwoD4CQ&usg=AFQjCNFuW6TgzE4QpahO_U7g3f3wdwecAg&sig2=iET-R0y9on_Pbflzf9aYDw&bvm=bv.71778758,bs.1,d.bGQ>`__
-  ARM v7-M Architecture Reference Manual (Available on the
   ARM site after a simple registration procedure. Also available on academic sites but beware of out of date versions.)
