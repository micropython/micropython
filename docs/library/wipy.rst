*************************************
:mod:`wipy` -- WiPy specific features
*************************************

.. module:: wipy
   :synopsis: WiPy specific features

The ``wipy`` module contains functions to control specific features of the
WiPy, such as the heartbeat LED.

Functions
---------

.. function:: heartbeat([enable])

   Get or set the state (enabled or disabled) of the heartbeat LED. Accepts and
   returns boolean values (``True`` or ``False``).
