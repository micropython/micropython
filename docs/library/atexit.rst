:mod:`atexit` -- exit handlers
======================================

.. module:: atexit
   :synopsis: The atexit module defines functions to register and unregister cleanup functions.

|see_cpython_module| :mod:`python:atexit`.

Functions
-------

.. function:: register(func)

   Register func as a function to be executed on :ref:`soft_reset` or exit on unix/windows ports.

.. function:: unregister(func)

   Remove func from the list of functions to be run at interpreter shutdown.
