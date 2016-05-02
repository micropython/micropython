:mod:`ucollections` -- collection and container types
=====================================================

.. module:: ucollections
   :synopsis: collection and container types

This module implements advanced collection and container types to
hold/accumulate various objects.

Classes
-------

.. function:: namedtuple(name, fields)

    This is factory function to create a new namedtuple type with a specific
    name and set of fields. A namedtyple is a subclass of tuple which allows
    to access its fields not just by numeric index, but also with an attribute
    access syntax using symbolic field names. Fields is a sequence of strings
    specifying field names. For compatibily with CPython it can also be a
    a string with space-separated field named (but this is less efficient).
    Example of use::

        from ucollections import namedtuple

        MyTuple = namedtuple("MyTuple", ("id", "name"))
        t1 = MyTuple(1, "foo")
        t2 = MyTuple(2, "bar")
        print(t1.name)
        assert t2.name == t2[1]

.. function:: OrderedDict(...)

    ``dict`` type subclass which remembers and preserves the order of keys
    added. When ordered dict is iterated over, keys/items are returned in
    the order they were added::

        from ucollections import OrderedDict

        # To make benefit of ordered keys, OrderedDict should be initialized
        # from sequence of (key, value) pairs.
        d = OrderedDict([("z", 1), ("a", 2)])
        # More items can be added as usual
        d["w"] = 5
        d["b"] = 3
        for k, v in d.items():
            print(k, v)

    Output::

        z 1
        a 2
        w 5
        b 3
