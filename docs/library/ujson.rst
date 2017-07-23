:mod:`ujson` -- JSON encoding and decoding
==========================================

.. module:: ujson
   :synopsis: JSON encoding and decoding

|see_cpython_module| :mod:`python:json`.

This modules allows to convert between Python objects and the JSON
data format.

Functions
---------

.. function:: dumps(obj)

   Return ``obj`` represented as a JSON string.

.. function:: loads(str)

   Parse the JSON ``str`` and return an object.  Raises ValueError if the
   string is not correctly formed.
