:mod:`select` -- wait for events on a set of streams
========================================================================

.. module:: select
   :synopsis: wait for events on a set of streams

This module provides functions to wait for events on streams (select streams
which are ready for operations).

Pyboard specifics
-----------------

Polling is an efficient way of waiting for read/write activity on multiple
objects.  Current objects that support polling are: :class:`pyb.UART`,
:class:`pyb.USB_VCP`.

Functions
---------

.. function:: poll()

   Create an instance of the Poll class.

.. function:: select(rlist, wlist, xlist[, timeout])

   Wait for activity on a set of objects.

   This function is provided for compatibility and is not efficient. Usage
   of :class:`Poll` is recommended instead.

.. _class: Poll

class ``Poll``
--------------

Methods
~~~~~~~

.. method:: poll.register(obj[, eventmask])

   Register ``obj`` for polling.  ``eventmask`` is 1 for read, 2 for
   write, 3 for read-write.

.. method:: poll.unregister(obj)

   Unregister ``obj`` from polling.

.. method:: poll.modify(obj, eventmask)

   Modify the ``eventmask`` for ``obj``.

.. method:: poll.poll([timeout])

   Wait for at least one of the registered objects to become ready. Returns
   list of ready objects, or empty list on timeout.

   Timeout is in milliseconds.
