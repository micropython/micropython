.. _memorymanagement:

Memory Management
=================

Unlike programming languages such as C/C++, MicroPython hides memory management 
details from the developer by supporting automatic memory management.
Automatic memory management is a technique used by operating systems or applications to automatically manage 
the allocation and deallocation of memory. This eliminates challenges such as forgetting to
free the memory allocated to an object. Automatic memory management also avoids the critical issue of using memory
that is already released. Automatic memory management takes many forms, one of them being
garbage collection (GC).

The garbage collector usually has two responsibilities;

#. Allocate new objects in available memory.
#. Free unused memory.

There are many GC algorithms but MicroPython uses the 
`Mark and Sweep <https://en.wikipedia.org/wiki/Tracing_garbage_collection#Basic_algorithm>`_
policy for managing memory. This algorithm has a mark phase that traverses the heap marking all
live objects while the sweep phase goes through the heap reclaiming all unmarked objects.

Garbage collection functionality in MicroPython is available through the ``gc`` built-in
module:

.. code-block:: bash
   
   >>> x = 5
   >>> x
   5
   >>> import gc
   >>> gc.enable()
   >>> gc.mem_alloc()
   1312
   >>> gc.mem_free()
   2071392
   >>> gc.collect()
   19
   >>> gc.disable()
   >>> 

Even when ``gc.disable()`` is invoked, collection can be triggered with ``gc.collect()``.

The object model
-----------------

All MicroPython objects are referred to by the ``mp_obj_t`` data type.
This can be "word-sized" (i.e. the same size as a pointer on the target architecture),
and can be typically 32-bit (STM32, nRF, ESP32, Unix x86) or 64-bit (Unix x64).
It can also be greater than a word-size, eg ``OBJ_REPR_D`` on a 32-bit architecture.

An ``mp_obj_t`` represents a MicroPython object, for example an integer, float, type, dict or
class instance. Some objects, like booleans and small integers, have their value stored directly
in the ``mp_obj_t`` value and do not require additional memory. Other objects have their value
store elsewhere in memory (for example on the garbage-collected heap) and their ``mp_obj_t`` contains
a pointer to that memory. A portion of ``mp_obj_t`` is the tag which tells what type of object it is.

.. note::
   This is specifically the case with ``OBJ_REPR_D``, which is used on Unix 64-bit.
   See ``mpconfig.h`` for the specific details of the available representations.

**Pointer tagging**

Because pointers are stored in an ``mp_obj_t`` are word-aligned, the lower bits will be zero.
i.e. on 64-bit, the lower 3 bits will be zero:

``********|********|********|********|********|********|********|********000``

These bits are reserved for purposes of storing a tag. A tag is a place holder that is used
to store extra information as opposed to introducing a new field to store information usually 
in the object which may be inefficient. 

The tags can store information like if we are dealing with a small integer, interned (small)
string or a concrete object as different semantics apply to each of these.

For small integers the mapping is this:

``********|..|******01``

For a small or interned string:

``********|..|******10``

While a concrete object that is neither a small integer nor an interned string takes this form:

``********|..|******00``

The stars in these 3 representations correspond to a small-int value, or a pointer/address.

Allocation of objects
----------------------

Small integers take up 8 bytes and will be allocated on the stack and not the heap. This implies
that the allocation of such integers does not affect the heap. Similarly, interned strings are small - usually,
of a length less than 10 are stored as an array.

.. note::
   Most bare-metal ports are 32-bit so "8 bytes" here would be "4 bytes".

Everything else which is a concrete object is allocated on the heap and its object structure is such that
a field is reserved in the object header to store the type of the object.

.. code-block:: bash

    +++++++++++
    +         +
    + type    + object header
    +         +
    +++++++++++
    +         + object items
    +         +
    +         +
    +++++++++++
    

The heap's unit of allocation is a block that is to say the heap is further subdivided into blocks of 32 bytes. Another structure also allocated on the heap tracks the allocation of
objects in each block. This structure is called a *bitmap*.

.. image:: img/bitmap.png

The bitmap tracks whether a block is "free" or "in use" and use two bits to track this state 
for each block.

The mark-sweep garbage collector manages the objects allocated on the heap. 
See `py/gc.c <https://github.com/micropython/micropython/blob/master/py/gc.c>`_
for the full implementation of these details.

**Allocation: heap layout**

The heap is arranged such that it consists of blocks in pools. A block
can have different properties:

- *ATB(allocation table byte):* If set, then the block is a normal block
- *FREE:* Free block
- *HEAD:* Head of a chain of blocks
- *TAIL:* In the tail of a chain of blocks
- *MARK :* Marked head block
- *FTB(finaliser table byte):* If set, then the block has a finaliser
