:mod:`collections` --- High-performance container datatypes
===========================================================

.. module:: collections
   :synopsis: High-performance container datatypes.

----------------------------------------------


.. function:: collections.namedtuple(typename, field_names)

   Returns a new tuple subclass named `typename`. The new subclass is
   used to create tuple-like objects that have fields accessible by
   attribute lookup as well as being indexable and iterable. Instances
   of the subclass also have a helpful docstring (with `typename` and
   `field_names`) and a helpful ``__repr__()`` method which lists the
   tuple contents in a name=value format.

   The `field_names` are a single string with each fieldname separated
   by whitespace and/or commas, for example ``'x y'`` or ``'x,
   y'``. Alternatively, `field_names` can be a sequence of strings
   such as ``['x', 'y']``.

   .. code-block:: python

      >>> # Basic example
      >>> Point = namedtuple('Point', ['x', 'y'])
      >>> p = Point(11, y=22)     # instantiate with positional or keyword arguments
      >>> p[0] + p[1]             # indexable like the plain tuple (11, 22)
      33
      >>> x, y = p                # unpack like a regular tuple
      >>> x, y
      (11, 22)
      >>> p.x + p.y               # fields also accessible by name
      33
      >>> p                       # readable __repr__ with a name=value style
      Point(x=11, y=22)


.. class:: collections.OrderedDict([items])

   Return an instance of a dict subclass, supporting the usual `dict`
   methods. An OrderedDict is a dict that remembers the order that
   keys were first inserted. If a new entry overwrites an existing
   entry, the original insertion position is left unchanged. Deleting
   an entry and reinserting it will move it to the end.


   .. method:: popitem(last=True)

      The ``popitem()`` method for ordered dictionaries returns and
      removes a ``(key, value)`` pair. The pairs are returned in LIFO
      order if last is true or FIFO order if false.
