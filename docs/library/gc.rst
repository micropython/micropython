:mod:`gc` -- control the garbage collector
==========================================

.. module:: gc
   :synopsis: control the garbage collector which automatically frees
              :ref:`heap memory <heap>`

|see_cpython_module| :mod:`python:gc`.

Functions
---------

.. function:: enable()

   Enable automatic garbage collection.

.. function:: disable()

   Disable automatic garbage collection.  Heap memory can still be allocated,
   and garbage collection can still be initiated manually using :meth:`gc.collect`.

.. function:: isenabled()

   Returns True if automatic garbage collection is enabled, and False otherwise.

.. function:: collect()

   Run a garbage collection.

.. function:: mem_alloc()

   Return the number of bytes of heap RAM that are allocated by Python code.

   .. admonition:: Difference to CPython
      :class: attention

      This function is MicroPython extension.

.. function:: mem_free()

   Return the number of bytes of heap RAM that is available for Python
   code to allocate, or -1 if this amount is not known.

   .. admonition:: Difference to CPython
      :class: attention

      This function is MicroPython extension.

.. function:: threshold([amount])

   Set or query the additional GC allocation threshold. Normally, a collection
   is triggered only when a new allocation cannot be satisfied, i.e. on an
   out-of-memory (OOM) condition. If this function is called, in addition to
   OOM, a collection will be triggered each time after *amount* bytes have been
   allocated (in total, since the previous time such an amount of bytes
   have been allocated). *amount* is usually specified as less than the
   full heap size, with the intention to trigger a collection earlier than when the
   heap becomes exhausted, and in the hope that an early collection will prevent
   excessive memory fragmentation. This is a heuristic measure, the effect
   of which will vary from application to application, as well as
   the optimal value of the *amount* parameter.

   Calling the function without argument will return the current value of
   the threshold. A value of -1 means a disabled allocation threshold.

   .. admonition:: Difference to CPython
      :class: attention

      This function is a MicroPython extension. CPython has a similar
      function - ``set_threshold()``, but due to different GC
      implementations, its signature and semantics are different.

   Examples
   ^^^^^^^^

   To trigger a garbage collection each time 32768 bytes of RAM have been allocated in total::

       gc.threshold(32768)

   To restore the default behaviour, only triggering garbage collection when out of memory::

       gc.threshold(-1)

Example
-------

.. code-block:: bash

    >>> import gc
    >>> gc.mem_free() # Gets number of bytes free in memory
    8192
    >>> gc.mem_alloc() # Gets number of bytes allocated in memory
    1024
    >>> foo = bytearray(1000) # Create a big array of data
    >>> gc.mem_free() # Show that there's less memory available
    7168
    >>> gc.mem_alloc() # Show that there's more memory used
    2048
    >>> del foo # Delete the object
    >>> gc.mem_free() # Show that collection hasn't run yet
    7168
    >>> gc.mem_alloc() # That memory is still allocated
    2048
    >>> gc.collect() # Manually run the collection
    >>> gc.mem_free() # Now we have reclaimed that memory
    8192
    >>> gc.mem_alloc() # That memory is no longer allocated
    1024
