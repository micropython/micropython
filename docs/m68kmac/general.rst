.. _m68kmac_general:

General information about the M68k Macport
==========================================

The Macintosh was an extremely influential computer since its introduction in 1984.
These computers are based on the Motorola 68000 family of CPUs.

This port targets systems with 2+ MB RAM running System 6 or System 7.

Mac hardware includes a full range of hardware devices: mouse, keyboard, display,
storage, and sometimes networking.


Macintosh APIs
--------------

There is a comprehensive, machine readable reference for the Macintosh operating system APIs:
`multiversal <https://github.com/autc04/multiversal>`_. The build process uses these to
produce Python bindings. For instance, the :mod:`qd` module is generated from the
`QuickDraw.yaml <https://github.com/autc04/multiversal/blob/master/defs/CQuickDraw.yaml>`_
specification file.

Extensions to uctypes
---------------------

The m68kmac micropython fork adds support for named types to micropython's :mod:`uctypes`.
For instance, ``mactypes.Rect`` is a named structure with the same memory layout as
the Macintosh ``Rect`` type.

To create a new ``Rect`` on the MicroPython heap, call the rect constructor. You can initialize fields using kwargs: ``r = Rect(top=48, left=8, bottom=308, right=504)``.

To treat a memory region as a ``Rect``, use ``Rect`` as the descriptor argument of :fun:`uctypes.struct`: ``uctypes.struct(addr, Rect)``.

A copy of a struct can be made with ``dest = src[:]`` and a struct can be updated in place with ``dest[:] = src``. The normal assignment operator ``dest = src`` merely makes ``dest`` refer to the same storage as ``src``. (Note that ``dest[:] = src`` does not currently type-check, but it may be made to do so in the future)
