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

   .. admonition:: Difference to CPython
      :class: attention

      This function is MicroPython extension.

.. function:: mem_free()

   Return the number of bytes of available heap RAM, or -1 if this amount
   is not known.

   .. admonition:: Difference to CPython
      :class: attention

      This function is MicroPython extension.

.. function:: threshold([amount])

   Set or query additional GC allocation threshold. Normally, GC is
   triggered when new allocation cannot be satisfied, i.e. on out of
   memory (OOM) condition. If this function is called, in addition to
   OOM, GC will be triggered each time after *amount* of bytes has been
   allocated (in total, since the previous time such amount of bytes
   had been allocated). *amount* is usually specified as less than the
   full heap size, with the intention to trigger GC earlier than the
   heap will be exhausted, and in the hope that early GC will prevent
   excessive memory fragmentation. This is a heuristic measure, effect
   of which will vary from an application to application, as well as
   the optimal value of *amount* parameter.

   Calling the function without argument will return current value of
   the threshold. Value of -1 means a disabled allocation threshold.

   .. admonition:: Difference to CPython
      :class: attention

      This function is MicroPython extension. CPython has a similar
      function - ``set_threshold()``, but due to different GC
      implementations, its signature and semantics are different.
