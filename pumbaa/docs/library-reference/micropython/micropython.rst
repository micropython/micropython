:mod:`micropython` --- Access and control MicroPython internals
===============================================================

.. module:: micropython
   :synopsis: Access and control MicroPython internals.

----------------------------------------------


.. function:: micropython.mem_info([verbose])

   Print information about currently used memory. If the `verbose`
   argument is given then extra information is printed.

   The information that is printed is implementation dependent, but
   currently includes the amount of stack and heap used. In verbose
   mode it prints out the entire heap indicating which blocks are used
   and which are free.


.. function:: micropython.qstr_info([verbose])

   Print information about currently interned strings. If the
   `verbose` argument is given then extra information is printed.

   The information that is printed is implementation dependent, but
   currently includes the number of interned strings and the amount of
   RAM they use. In verbose mode it prints out the names of all
   RAM-interned strings.


.. function:: micropython.alloc_emergency_exception_buf(size)

   Allocate `size` bytes of RAM for the emergency exception buffer (a
   good size is around 100 bytes). The buffer is used to create
   exceptions in cases when normal RAM allocation would fail (eg
   within an interrupt handler) and therefore give useful traceback
   information in these situations.

   A good way to use this function is to put it at the start of your
   main script (eg main.py) and then the emergency exception buffer
   will be active for all the code following it.
