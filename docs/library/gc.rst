:mod:`gc` -- control the garbage collector
==========================================

.. module:: gc
   :synopsis: control the garbage collector

Functions
---------

.. function:: enable()

   Enable automatic garbage collection.

.. function:: disable()

   Disable automatic garbage collection.  Heap memory can still be allocated,
   and garbage collection can still be initiated manually using :meth:`gc.collect`.

.. function:: collect()

   Run a garbage collection.

.. function:: mem_alloc()

   Return the number of bytes of heap RAM that are allocated.

.. function:: mem_free()

   Return the number of bytes of available heap RAM.
