:mod:`select` --- Waiting for I/O completion
============================================

.. module:: select
   :synopsis: Waiting for I/O completion.

This module provides access to the `poll()` function available in most
operating systems. It only works for the Simba channels;
:class:`events<sync.sync.Event>`, :class:`queues<sync.sync.Queue>` and
:class:`sockets<socket.socket.SocketType>`.

----------------------------------------------


.. function:: select.poll()

   Returns a polling object, which supports registering and
   unregistering channels, and then polling them for I/O events.


.. class:: select.poll()

   The poll class.

   .. code-block:: python

      >>> from sync import Queue
      >>> poll = select.poll()
      >>> queue = Queue()
      >>> poll.register(queue)
      >>> poll.poll(1.0)             # Timeout since no data is available in the queue.
      []
      >>> queue.write(b'hello')
      >>> poll.poll()                # Data is available in the queue.
      [(<0x20034050>, 1)]
      >>> poll.poll()[0][0].read(5)  # Read the available data.
      b'hello'


   .. method:: register(channel[, eventmask])

      Register given `channel` with the polling object. Future calls
      to the `poll()` method will then check whether the channel has
      any pending I/O events.

      `eventmask` is an optional bitmask describing the type of events
      you want to check for, and can be currently only be ``POLLIN``.

      Registering a file descriptor that’s already registered is not
      an error, and has the same effect as registering the descriptor
      exactly once.


   .. method:: unregister(channel)

      Remove given `channel` being tracked by a polling object.

      Attempting to remove a file descriptor that was never registered
      causes a `KeyError` exception to be raised.


   .. method:: modify(channel, eventmask)

      Modifies an already registered channel. This has the same effect
      as `register(channel, eventmask)`. Attempting to modify a
      channel that was never registered causes an `IOError` exception
      with errno `ENOENT` to be raised.


   .. method:: poll([timeout])

      Polls the set of registered channels, and returns a
      possibly-empty list containing ``(channel, event)`` 2-tuples for
      the descriptors that have events or errors to report. An empty
      list indicates that the call timed out and no channel had any
      events to report. If `timeout` is given, it specifies the length
      of time in milliseconds which the system will wait for events
      before returning. If `timeout` is omitted, negative, or
      ``None``, the call will block until there is an event for this
      poll object.


.. data:: select.POLLIN

   There is data to read.


.. data:: select.POLLHUP

   Hung up.
