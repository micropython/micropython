:mod:`enum` -- MicroPython Enum class like in the Python
========================================================

.. module:: enum
   :synopsis: MicroPython Enum class like in the Python

This module implements enumeration Enum class for the MicroPython.

class Enum -- enumeration
-------------------------

Class Enum is based in dictionary `dict` class and provides a way to create enumeration
objects with key-value pairs.

Constructor
-----------

.. class:: Enum(arg)

   Construct and return a new Enum object using the following parameter:

      - *arg* is the dictionary of key-value pairs.

Usage example::

    from enum import Enum
    e = Enum({"X": 1, "Y": 2})  # create Enum object from dictionary of key-value pairs
    print(e)
    e.A = 'a'                   # add new key-value pair
    e.B = 'b'
    print(e)
    del e.B                     # delete key-value pair
    print(e)
    print(e.X)                  # get value from key
    print(e.key_from_value(2))  # get key from value
    print(e.B)                  # raise an exception due to no such a key attribute

Output::

    Enum({'X':1, 'Y':2})
    Enum({'A':'a', 'B':'b', 'X':1, 'Y':2})
    Enum({'A':'a', 'X':1, 'Y':2})
    1
    Y
    KeyError: No such attribute: B

Methods
-------

.. _Enum_append:
.. method:: Enum.append(*arg, **enums):

    Append key-value pairs with the following parameters:

      - *arg* is the dictionary or tuple of dictionaryes or list of dictionaryes: [dict | tuple(dict1, dict2, ...) | list(dict1, dict2, ...)]
      - *kw_args* are key1=value1, key2=value2, ...

.. method:: Enum.is_value(value)

    Check if value is in the Enum object.
    Return True if value is in the Enum object, otherwise False.

.. method:: Enum.key_from_value(value)

    Get key from value. If value is not in the Enum object, raise an exception.

Usage example::

    from enum import Enum

    class State(Enum):
        Stop = 10
        Run = 20
        Ready = 30


    state = State()
    print("state:", State())

    current_state = state.Stop
    print("current_state:", current_state, state.key_from_value(current_state))
    if current_state == state.Stop:
        print(" Stop state")
    if current_state != state.Ready:
        print(" Not a Ready state")
        print(" Run!")
        current_state = state.Run
    print("current_state:", current_state, state.key_from_value(current_state))
    # some process
    i = -1
    while current_state != state.Ready:
        i += 1
        if state.is_value(i):
            if int(str(State(i))) == state.Ready:
                current_state = state.Ready
        print(".", end="")
    print()
    print("current_state:", current_state, state.key_from_value(current_state))
    print("Done!")

Output::

    state: State({'Run':20, 'Stop':10, 'Ready':30})
    current_state: 10 State.Stop
     Stop state
     Not a Ready state
     Run!
    current_state: 20 State.Run
    ...............................
    current_state: 30 State.Ready
    Done!

.. warning::

    There is the trouble with the isinstance()/type().

    >>> isinstance(State.Run, State)
    False
    >>> State(State.Ready) == State.Ready
    False
    >>> int(str(State(State.Ready))) == State.Ready
    True
    >>> type(State(State.Ready))
    <class 'State'>
    >>> type(state(State.Ready))
    <class 'int'>
    >>> state(State.Ready) == State.Ready
    True

Function
--------

.. function:: enum(*arg, **kw_args)

    Return Enum object using the following parameters: see :ref:`Enum.append <Enum_append>` method.

Usage examples::

    from enum import Enum
    e = enum()
    e
    e = enum(X=1, Y=2)
    e
    e = enum({'X':1, 'Y':2})
    e
    e = enum(({'X':1, 'Y':2}, {'A':'a', 'B':'b'}, ))
    e
    e = enum([{'X':1, 'Y':2}, {'A':'a', 'B':'b'}, ])
    e

Output::

    Enum()
    Enum({'X':1, 'Y':2})
    Enum({'X':1, 'Y':2})
    Enum({ 'A':'a', 'B':'b', 'X':1, 'Y':2})
    Enum({ 'A':'a', 'B':'b', 'X':1, 'Y':2})

.. note::

    ZZZ.

.. seealso::

..
    :ref:`enum_test.py <enum_test.py>`

    :ref:`enum.py <enum.py>`
