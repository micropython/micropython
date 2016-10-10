:mod:`sync` --- Thread synchroniztion
=====================================

.. module:: sync
   :synopsis: Thread synchroniztion.

Thread synchronization refers to the idea that multiple threads are to
join up or handshake at a certain point, in order to reach an
agreement or commit to a certain sequence of action.

Simba documentation: `sync`_

----------------------------------------------


.. class:: sync.Event()

   Initialize given event object.

   Simba documentation: `sync/event`_


   .. method:: read(mask)

      Wait for an event to occur and return a mask of all active
      events.


   .. method:: write(mask)

      Write given event(s) to the channel.


   .. method:: size()

      Get the number of event(s) set in the channel.


.. class:: sync.Queue()

   Initialize given queue object.

   Simba documentation: `sync/queue`_


   .. method:: read(size)

      Reads up to `size` number of bytes from the queue and returns
      them as a string. Raises an exception on error.


   .. method:: write(string)

      Write given `string` to the queue. Returns the number of bytes
      written. Raises an exception on error.


   .. method:: size()

      Get the number of bytes available to read.


.. _sync: http://simba-os.readthedocs.io/en/latest/library-reference/sync.html
.. _sync/event: http://simba-os.readthedocs.io/en/latest/library-reference/sync/event.html
.. _sync/queue: http://simba-os.readthedocs.io/en/latest/library-reference/sync/queue.html
