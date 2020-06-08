:mod:`uevent` -- wait for events on a set of objects
====================================================

.. module:: uevent
   :synopsis: wait for events on a set of objects

This class is similar to the ``select`` and ``selectors`` modules but:
- poll.register() will update flags, not overwrite
- one-shot behaviour
- poll_entry can have a user method set on it
- everything can be O(1) in a native implementation
- poll.notify() for async wake-up (TODO)
- helps to minimise heap allocation, both in this module and for the user's use of it
- unregister operates on the poll_entry (also on poll)

Functions
---------

.. function:: poll()

   Create an instance of the Poll class.

.. _class: Poll

class ``Poll``
--------------

Methods
~~~~~~~

.. method:: poll.register(obj[, eventmask])

.. method:: poll.unregister(obj[, eventmask])

.. method:: poll.poll_ms(timeout=-1, /)
