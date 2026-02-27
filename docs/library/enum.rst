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
    e = Enum({"X": 1, "Y": 2})                          # create Enum object from dictionary of key-value pairs
    print(e)
    print("add new key-value pair")
    e.A = 'A'                                           # add new key-value pair
    e.B = 'B'
    print(e)
    print("e.X:", e.X)                                  # get value from key
    print("e.X.value:", e.X.value)                      # get value from key
    print("e(e.X):", e(e.X))                            # get value from key
    print("e.key_from_value(1):", e.key_from_value(1))  # get key from value
    print("e.is_value(e.B):", e.is_value(e.B))
    print("del e.B")
    del e.B                                             # delete key-value pair
    print(e)
    print("e.is_value('B'):", e.is_value('B'))          # check if the value is in the Enum object
    print("e.B: will raise the KeyError exception")
    print(e.B)                                          # raise an exception due to no such a key attribute

Output is::

    Enum({'Y': 2, 'X': 1})
    add new key-value pair
    Enum({'A': 'A', 'B': 'B', 'Y': 2, 'X': 1})
    e.X: 1
    e.X.value: 1
    e(e.X): 1
    e.key_from_value(1): Enum.X
    e.is_value(e.B): True
    del e.B
    Enum({'A': 'A', 'Y': 2, 'X': 1})
    e.is_value('B'): False
    e.B: will raise the KeyError exception
    Traceback (most recent call last):
    File "<stdin>", line 234, in <module>
    File "<stdin>", line 126, in __getattr__
    KeyError: no such attribute: B

Methods
-------

.. _Enum_append:
.. method:: Enum.append(arg, **kw_args):

    Append key-value pairs with the following parameters:

      - *arg* is the dictionary of key-value pairs.
      - *kw_args* are key1=value1, key2=value2, ...

.. method:: Enum.is_value(value)

    Check if the value is in the Enum object.
    Returns True if the value is in the Enum object, and False otherwise.

.. method:: Enum.key_from_value(value)

    Returns the key from the value. If a value is not in the Enum object, it raises an exception.

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
            if state(i) == state.Ready:
                current_state = state.Ready
        print(".", end="")
    print()
    print("current_state:", current_state, state.key_from_value(current_state))
    print("Done!")

Output is::

    state: State({'Ready': 30, 'Stop': 10, 'Run': 20})
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

.. function:: enum(**kw_args)

    Returns an Enum object from the following parameters:

      - *kw_args* are key1=value1, key2=value2, ...

Usage examples::

    from enum import enum
    e = enum()
    repr(e)
    str(e)
    e = enum(X=1, Y=2.0, A='A', ON=True)
    repr(e)
    str(e)
    e.X
    e.Y
    e.A
    e.ON

Output::

    '{}'
    'Enum({})'
    "{'A': 'A', 'ON': True, 'Y': 2.0, 'X': 1}"
    "Enum({'A': 'A', 'ON': True, 'Y': 2.0, 'X': 1})"
    1
    2.0
    'A'
    True

.. note::

    ABC

.. seealso::

    See `enum_test.py <https://github.com/micropython/micropython-lib/pull/980/files/>`_ and `enum.py <https://github.com/micropython/micropython-lib/pull/980/files/>`_
