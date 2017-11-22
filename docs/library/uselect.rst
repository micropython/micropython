:mod:`uselect` -- wait for events on a set of streams
========================================================================

.. module:: uselect
   :synopsis: wait for events on a set of streams

|see_cpython_module| :mod:`python:select`.

This module provides functions to efficiently wait for events on multiple
streams (select streams which are ready for operations).

Functions
---------

.. function:: poll()

   Create an instance of the Poll class.

.. function:: select(rlist, wlist, xlist[, timeout])

   Wait for activity on a set of objects.

   This function is provided by some MicroPython ports for compatibility
   and is not efficient. Usage of :class:`Poll` is recommended instead.

.. _class: Poll

class ``Poll``
--------------

Methods
~~~~~~~

.. method:: poll.register(obj[, eventmask])

   Register *obj* for polling. *eventmask* is logical OR of:

   * ``select.POLLIN``  - data available for reading
   * ``select.POLLOUT`` - more data can be written
   * ``select.POLLERR`` - error occurred
   * ``select.POLLHUP`` - end of stream/connection termination detected

   *eventmask* defaults to ``select.POLLIN | select.POLLOUT``.

.. method:: poll.unregister(obj)

   Unregister *obj* from polling.

.. method:: poll.modify(obj, eventmask)

   Modify the *eventmask* for *obj*.

.. method:: poll.poll([timeout])

   Wait for at least one of the registered objects to become ready. Returns
   list of (``obj``, ``event``, ...) tuples, ``event`` element specifies
   which events happened with a stream and is a combination of ``select.POLL*``
   constants described above. There may be other elements in tuple, depending
   on a platform and version, so don't assume that its size is 2. In case of
   timeout, an empty list is returned.

   Timeout is in milliseconds.

   .. admonition:: Difference to CPython
      :class: attention

      Tuples returned may contain more than 2 elements as described above.

.. method:: poll.ipoll(timeout=-1, flags=0)

   Like :meth:`poll.poll`, but instead returns an iterator which yields
   `callee-owned tuples`. This function provides efficient, allocation-free
   way to poll on streams.

   If *flags* is 1, one-shot behavior for events is employed: streams for
   which events happened, event mask will be automatically reset (equivalent
   to ``poll.modify(obj, 0)``), so new events for such a stream won't be
   processed until new mask is set with `poll.modify()`. This behavior is
   useful for asynchronous I/O schedulers.

   .. admonition:: Difference to CPython
      :class: attention

      This function is a MicroPython extension.
