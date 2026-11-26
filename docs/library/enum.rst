:mod:`enum` -- MicroPython Enum class like in the Python
========================================================

.. module:: enum
   :synopsis: MicroPython Enum class like in the Python

This module implements enumeration Enum class for the MicroPython.

class Enum -- enumeration
-------------------------

Base class for creating enumerated constants as a set of symbolic names(members) bound to unique values.


Constructor
-----------

.. class:: Enum()
           Enum(name, names)
           Enum(value)

   Construct and return a new Enum object using the following parameter:

      - *name* is the name of the Enum class.
      - *names* is the dictionary of key-value pairs.
      - *value* is the key or value. Member is returned if the value is in the Enum class, and ValueError is raised otherwise.

Methods
-------

.. method:: Enum.list()

    Return the list of members of the Enum class.

.. method:: Enum.is_value(value)

    Check if the value is in the Enum class.
    Returns True if the value is in the Enum class, and False otherwise.

Usage example::

    from enum import Enum

    # Class syntax
    class Color(Enum):
        RED = 1
        GREEN = 2
        BLUE = 3

    color = Color()

    # or

    # Functional syntax
    color = Enum('Color', {'RED': 1, 'GREEN': 2, 'BLUE': 3})

    print(color.list())
    print(color('GREEN'))
    print(color(2))
    print(color.GREEN)
    print(color.GREEN.name)
    print(color.GREEN.value)
    print(len(color))
    print([m for m in color])

Output is::

    [Color.RED: 1, Color.GREEN: 2, Color.BLUE: 3]
    Color.GREEN: 2
    Color.GREEN: 2
    Color.GREEN: 2
    GREEN
    2
    3
    [Color.RED: 1, Color.GREEN: 2, Color.BLUE: 3]

Usage example::

    from enum import Enum

    class State(Enum):
        Stop = 10
        Run = 20
        Ready = 30

    state = State()
    print("state:", State())

    current_state = state.Stop
    print("current_state:", current_state, state(current_state))
    if current_state == state.Stop:
        print(" Stop state")
    if current_state != state.Ready:
        print(" Not a Ready state")
        print(" Run!")
        current_state = state.Run
    print("current_state:", current_state, state(current_state))
    # some process
    i = -1
    while current_state != state.Ready:
        i += 1
        if state.is_value(i):
            if state(i) == state.Ready:
                current_state = state.Ready
        print(".", end="")
    print()
    print("current_state:", current_state, state(current_state))
    print("Done!")

Output is::

    state: Enum(name='State', names={'Run': 20, 'Stop': 10, 'Ready': 30})
    current_state: State.Stop: 10 State.Stop: 10
    Stop state
    Not a Ready state
    Run!
    current_state: State.Run: 20 State.Run: 20
    ...............................
    current_state: State.Ready: 30 State.Ready: 30
    Done!

.. warning::

    ABC

.. note::

    ABC

.. seealso::

    See `enum_test.py <https://github.com/micropython/micropython-lib/pull/980/files/>`_ and `enum.py <https://github.com/micropython/micropython-lib/pull/980/files/>`_
