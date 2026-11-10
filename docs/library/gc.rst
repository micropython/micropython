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

Heap areas
----------

On some boards the heap is made of several separate areas of memory, for
example internal SRAM banks and external PSRAM or SDRAM. These differ in speed,
and some peripherals can only use some of them, for example with DMA. The
following functions show the areas and let Python code choose which areas its
allocations go in. They are only available when the port enables
``MICROPY_GC_AREA_SELECT``, which needs a heap with more than one area.

Areas are numbered from 0 in the order the port adds them. Area 0 is the first
area of the heap, and is usually the main internal RAM. Without a selection,
allocations go in the first area with enough free memory, as before.

.. admonition:: Difference to CPython
   :class: attention

   These functions and constants are MicroPython extensions.

.. function:: areas()

   Return a list with one tuple per heap area, in index order. Each tuple is
   ``(start, total, free, max_free, attrs)``:

   - *start* is the address of the area's first allocatable byte.
   - *total* is the number of allocatable bytes in the area.
   - *free* is the number of free bytes in the area.
   - *max_free* is the size in bytes of the largest free run in the area,
     which is the largest single allocation that can currently succeed there.
   - *attrs* is the area's attributes, a bitwise OR of the ``AREA_xxx``
     constants below. Ports set the attributes when they add an area.

   *free* and *max_free* are measured when this function runs, and change as
   objects are allocated and collected.

.. function:: area(*indices, attrs=0, strict=False, align=0)

   Return a context manager that makes allocations by the current thread go
   in the given heap areas, and optionally with a given alignment, while it is
   active. The previous settings are restored when the ``with`` block ends,
   including when it ends with an exception, so blocks can be nested.

   - *indices* are area indices as shown by `areas()`.
   - *attrs* selects the areas that have all of the given attributes. If
     *indices* are also given, only the listed areas that have the attributes
     are selected.
   - If *strict* is false, the selected areas are tried first, and the other
     areas are used when the selected ones are full, so the selection never
     causes a `MemoryError` that wouldn't happen otherwise. If *strict* is
     true, an allocation that doesn't fit in the selected areas raises
     `MemoryError`, after a garbage collection, rather than going in another
     area. Use this when memory must be in a particular area, for example a
     buffer that a peripheral accesses with DMA.
   - *align*, if not 0, is a power of two. Each allocation then starts at an
     address that is a multiple of *align*, and uses a multiple of *align*
     bytes, so it doesn't share an aligned unit of memory with any other
     object. For example, use the CPU's cache line size for a buffer that DMA
     accesses on a microcontroller with a data cache. Alignments up to the
     heap's block size, usually 16 bytes, are always met.

   Selected areas are tried in index order. With no *indices* and no *attrs*,
   areas are chosen as if there were no selection, which is useful inside
   another ``with gc.area(...)`` block.

   `ValueError` is raised if an index doesn't exist, if no area has the given
   attributes, or if *align* is not a power of two.

   Every allocation made inside the block uses these settings, including
   allocations made by the interpreter itself, such as for an object's
   internal data or for an exception's traceback. Keep the block short. With
   ``strict=True``, these small allocations also fail once the selected areas
   are full, and with *align* they also use a whole number of aligned units.
   Callbacks run by the scheduler while the block is active also use the
   settings. Other threads are not affected.

   If an object grows after the block has ended, for example when a
   `bytearray` is extended, and its memory has to move, the new memory is
   taken from the same area if possible, but other areas are used if that
   area is full, and the alignment isn't kept. Grow the object inside a
   ``with gc.area(..., strict=True, align=...)`` block to keep both.

   For example, to allocate a frame buffer in external RAM if there is room, a
   buffer that must be in area 1, and a buffer aligned to 32-byte cache lines::

       import gc

       with gc.area(attrs=gc.AREA_EXTERNAL):
           frame = bytearray(320 * 240 * 2)

       with gc.area(1, strict=True):
           buf = bytearray(4096)

       with gc.area(align=32):
           dma_buf = bytearray(512)

.. function:: area_of(obj)

   Return the index of the heap area that holds *obj*, or ``None`` if it is not
   in the heap. For objects with a buffer, such as `bytearray` and
   `array.array`, this is the area of the buffer's data, which can be in a
   different area to the object itself.

.. data:: AREA_EXTERNAL

   Attribute of areas in memory outside the microcontroller, such as PSRAM or
   SDRAM. These are usually larger and slower than internal RAM.

.. data:: AREA_DMA

   Attribute of areas that the port's DMA controllers can access. Ports only
   set this where they know it to be true, so not every port sets it.

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
