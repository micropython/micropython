:mod:`select` -- Provides select function to wait for events on a stream
========================================================================

.. module:: select
   :synopsis: Provides select function to wait for events on a stream

This module provides the select function.

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

   Wait for one of the registered objects to become ready.

   Timeout is in milliseconds.
