:mod:`atexit` -- exit handlers
==============================

.. module:: atexit
   :synopsis: The atexit module defines functions to register and unregister cleanup functions.

|see_cpython_module| :mod:`python:atexit`.

Functions
---------

.. function:: register(func)

   Register func as a function to be executed on :ref:`soft_reset` or exit on unix/windows ports.
   Functions are called in LIFO (last in, first out) order.
   If a handler raises an exception, it is printed to stderr (unless it is a ``SystemExit``)
   and the remaining handlers continue to run.

.. function:: unregister(func)

   Remove func from the list of functions to be run at interpreter shutdown. Not enabled on all ports.
