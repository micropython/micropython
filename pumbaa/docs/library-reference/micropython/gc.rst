:mod:`gc` --- Garbage Collector management
==========================================

.. module:: gc
   :synopsis: Garbage Collector management.

----------------------------------------------


.. function:: gc.enable()

   Enable automatic garbage collection.


.. function:: gc.disable()

   Disable automatic garbage collection. Heap memory can still be
   allocated, and garbage collection can still be initiated manually
   using ``gc.collect()``.


.. function:: gc.collect()

   Run a garbage collection.


.. function:: gc.mem_alloc()

   Returns the number of bytes allocated on the heap.


.. function:: gc.mem_free()

   Returns the number of bytes available on the heap.
