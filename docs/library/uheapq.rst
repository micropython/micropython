:mod:`uheapq` -- heap queue algorithm
=====================================

.. module:: uheapq
   :synopsis: heap queue algorithm

This module implements the heap queue algorithm.

A heap queue is simply a list that has its elements stored in a certain way.

Functions
---------

.. function:: heappush(heap, item)

   Push the ``item`` onto the ``heap``.

.. function:: heappop(heap)

   Pop the first item from the ``heap``, and return it.  Raises IndexError if
   heap is empty.

.. function:: heapify(x)

   Convert the list ``x`` into a heap.  This is an in-place operation.
