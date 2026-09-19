:mod:`json` -- JSON encoding and decoding
=========================================

.. module:: json
   :synopsis: JSON encoding and decoding

|see_cpython_module| :mod:`python:json`.

This modules allows to convert between Python objects and the JSON
data format.

Functions
---------

.. function:: dump(obj, stream, separators=None, default=None)

   Serialise *obj* to a JSON string, writing it to the given *stream*.

   If specified, separators should be an ``(item_separator, key_separator)``
   tuple. The default is ``(', ', ': ')``. To get the most compact JSON
   representation, you should specify ``(',', ':')`` to eliminate whitespace.

   If specified, default should be an ``(obj) -> serializable_obj`` callable
   for objects that can't otherwise be serialized. It should return a JSON encodable
   version of the object or raise a ``TypeError``.

.. function:: dumps(obj, separators=None, default=None)

   Return *obj* represented as a JSON string.

   The arguments have the same meaning as in `dump`.

.. function:: load(stream)

   Parse the given *stream*, interpreting it as a JSON string and
   deserialising the data to a Python object.  The resulting object is
   returned.

   Parsing continues until end-of-file is encountered.
   A :exc:`ValueError` is raised if the data in *stream* is not correctly formed.

.. function:: loads(str)

   Parse the JSON *str* and return an object.  Raises :exc:`ValueError` if the
   string is not correctly formed.
