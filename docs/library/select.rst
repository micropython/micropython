:mod:`select` --- Provides select function to wait for events on a stream
=========================================================================

.. module:: select
   :synopsis: Provides select function to wait for events on a stream

This module provides the select function.


Functions
---------

.. function:: poll()


.. function:: select(rlist, wlist, xlist[, timeout])


class Poll
----------


Methods
-------

.. method:: poll.modify(obj, eventmask)


.. method:: poll.poll([timeout])

   Timeout is in milliseconds.

.. method:: poll.register(obj[, eventmask])


.. method:: poll.unregister(obj)

