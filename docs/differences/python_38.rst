.. _python_38:

Python 3.8
==========

Python 3.8.0 (final) was released on the 14 October 2019. The Features for 3.8
are defined in `PEP 569 <https://www.python.org/dev/peps/pep-0569/#id9>`_ and
a detailed description of the changes can be found in `What's New in Python
3.8. <https://docs.python.org/3/whatsnew/3.8.html>`_

.. table::
  :widths: 20 60 20

  +--------------------------------------------------------+---------------------------------------------------+--------------------+
  | **Features**                                                                                               | **Status**         |
  +--------------------------------------------------------+---------------------------------------------------+--------------------+
  | `PEP 570 <https://www.python.org/dev/peps/pep-0570/>`_ | Positional-only arguments                         | Not implemented    |
  +--------------------------------------------------------+---------------------------------------------------+--------------------+
  | `PEP 572 <https://www.python.org/dev/peps/pep-0572/>`_ | Assignment Expressions                            | Complete           |
  +--------------------------------------------------------+---------------------------------------------------+--------------------+
  | `PEP 574 <https://www.python.org/dev/peps/pep-0574/>`_ | Pickle protocol 5 with out-of-band data           | Not implemented    |
  +--------------------------------------------------------+---------------------------------------------------+--------------------+
  | `PEP 578 <https://www.python.org/dev/peps/pep-0578/>`_ | Runtime audit hooks                               | Not relevant       |
  +--------------------------------------------------------+---------------------------------------------------+--------------------+
  | `PEP 587 <https://www.python.org/dev/peps/pep-0587/>`_ | Python Initialization Configuration               | Not relevant       |
  +--------------------------------------------------------+---------------------------------------------------+--------------------+
  | `PEP 590 <https://www.python.org/dev/peps/pep-0590/>`_ | Vectorcall: a fast calling protocol for CPython   | Not relevant       |
  +--------------------------------------------------------+---------------------------------------------------+--------------------+
  | **Miscellaneous**                                                                                                               |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  |  f-strings support = for self-documenting expressions and debugging                                        | Partial [#fstr38]_ |
  +------------------------------------------------------------------------------------------------------------+--------------------+

Other Language Changes:

.. table::
  :widths: 90 10

  +------------------------------------------------------------------------------------------------------------+--------------------+
  | A *continue* statement was illegal in the *finally* clause due to a problem with the implementation. In    | Complete           |
  | Python 3.8 this restriction was lifted                                                                     |                    |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | The *bool*, *int* , and *fractions.Fraction* types now have an *as_integer_ratio()* method like that found | Not implemented    |
  | in *float* and *decimal.Decimal*                                                                           |                    |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | Constructors of *int*, *float* and *complex* will now use the *__index__()* special method, if available   | Not implemented    |
  | and the corresponding method *__int__()*, *__float__()* or *__complex__()* is not available                |                    |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | Added support of *\N{name}* escapes in regular expressions                                                 | Not implemented    |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | Dict and dictviews are now iterable in reversed insertion order using *reversed()*                         | Not implemented    |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | The syntax allowed for keyword names in function calls was further restricted. In particular,              | Not implemented    |
  | f((keyword)=arg) is no longer allowed                                                                      |                    |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | Generalized iterable unpacking in yield and return statements no longer requires enclosing parentheses     | Not implemented    |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | When a comma is missed in code such as [(10, 20) (30, 40)], the compiler displays a SyntaxWarning with a   | Not implemented    |
  | helpful suggestion                                                                                         |                    |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | Arithmetic operations between subclasses of *datetime.date* or *datetime.datetime* and *datetime.timedelta*| Not implemented    |
  | objects now return an instance of the subclass, rather than the base class                                 |                    |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | When the Python interpreter is interrupted by *Ctrl-C (SIGINT)* and the resulting *KeyboardInterrupt*      | Complete           |
  | exception is not caught, the Python process now exits via a SIGINT signal or with the correct exit code    |                    |
  | such that the calling process can detect that it died due to  a *Ctrl-C*                                   |                    |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | Some advanced styles of programming require updating the *types.CodeType* object for an existing function  | Not implemented    |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | For integers, the three-argument form of the pow() function now permits the exponent to be negative in the | Not implemented    |
  | case where the base is relatively prime to the modulus                                                     |                    |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | Dict comprehensions have been synced-up with dict literals so that the key is computed first and the value | Not implemented    |
  | second                                                                                                     |                    |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | The *object.__reduce__()* method can now return a tuple from two to six elements long                      | Not implemented    |
  +------------------------------------------------------------------------------------------------------------+--------------------+

Changes to built-in modules:

.. table::
  :widths: 90 10

  +------------------------------------------------------------------------------------------------------------+--------------------+
  | `asyncio <https://docs.python.org/3/whatsnew/3.8.html#asyncio>`_                                                                |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | *asyncio.run()* has graduated from the provisional to stable API                                           | Complete           |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | Running *python -m asyncio* launches a natively async REPL                                                 | Not implemented    |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | The exception *asyncio.CancelledError* now inherits from *BaseException* rather than *Exception* and no    | Complete           |
  | longer inherits from *concurrent.futures.CancelledError*                                                   |                    |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | Added *asyncio.Task.get_coro()* for getting the wrapped coroutine within an *asyncio.Task*                 | Partial [#coro38]_ |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | Asyncio tasks can now be named, either by passing the name keyword argument to *asyncio.create_task()* or  | Not implemented    |
  | the *create_task()* event loop method, or by calling the *set_name()* method on the task object            |                    |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | Added support for Happy Eyeballs to *asyncio.loop.create_connection()*. To specify the behavior, two new   | Not implemented    |
  | parameters have been added: *happy_eyeballs_delay* and interleave.                                         |                    |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | `gc <https://docs.python.org/3/whatsnew/3.8.html#gc>`_                                                                          |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | *get_objects()* can now receive an optional generation parameter indicating a generation to get objects    | Not implemented    |
  | from. (Note, though, that while *gc* is a built-in, *get_objects()* is not implemented for MicroPython)    |                    |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | `math <https://docs.python.org/3/whatsnew/3.8.html#math>`_                                                                      |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | Added new function *math.dist()* for computing Euclidean distance between two points                       | Not implemented    |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | Expanded the *math.hypot()* function to handle multiple dimensions                                         | Not implemented    |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | Added new function, *math.prod()*, as analogous function to *sum()* that returns the product of a "start"  | Not implemented    |
  | value (default: 1) times an iterable of numbers                                                            |                    |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | Added two new combinatoric functions *math.perm()* and *math.comb()*                                       | Not implemented    |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | Added a new function *math.isqrt()* for computing accurate integer square roots without conversion to      | Not implemented    |
  | floating point                                                                                             |                    |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | The function *math.factorial()* no longer accepts arguments that are not int-like                          | Complete           |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | `sys <https://docs.python.org/3/whatsnew/3.8.html#sys>`_                                                                        |
  +------------------------------------------------------------------------------------------------------------+--------------------+
  | Add new *sys.unraisablehook()* function which can be overridden to control how "unraisable exceptions"     | Not implemented    |
  | are handled                                                                                                |                    |
  +------------------------------------------------------------------------------------------------------------+--------------------+

.. rubric:: Notes

.. [#fstr38] The ``=`` specifier is supported, but the interpolated value is
   formatted with ``str()`` rather than with the implicit ``repr()`` that
   CPython uses, and whitespace around the ``=`` (eg ``f'{x = }'``) is not
   supported.

.. [#coro38] There is no ``get_coro()`` method.  The wrapped coroutine is
   instead reachable via the undocumented ``Task.coro`` attribute.
