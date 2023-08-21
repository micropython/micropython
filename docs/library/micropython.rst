:mod:`micropython` -- MicroPython extensions and internals
==========================================================

.. module:: micropython
   :synopsis: access and control MicroPython internals

Functions
---------

.. function:: const(expr)

   Used to declare that the expression is a constant so that the compiler can
   optimise it.  The use of this function should be as follows::

    from micropython import const

    CONST_X = const(123)
    CONST_Y = const(2 * CONST_X + 1)

   Constants declared this way are still accessible as global variables from
   outside the module they are declared in.  On the other hand, if a constant
   begins with an underscore then it is hidden, it is not available as a global
   variable, and does not take up any memory during execution.

   This `const` function is recognised directly by the MicroPython parser and is
   provided as part of the :mod:`micropython` module mainly so that scripts can be
   written which run under both CPython and MicroPython, by following the above
   pattern.
