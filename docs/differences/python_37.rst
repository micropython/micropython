.. _python_37:


***********
Python 3.7
***********

New Features:

  +--------------------------------------------------------+--------------------------------------------------+----------------+
  |**Features**                                                                                               | Status         |
  +--------------------------------------------------------+--------------------------------------------------+----------------+
  | `PEP 538 <https://www.python.org/dev/peps/pep-0538/>`_ | Coercing the legacy C locale to a UTF-8 based    | Completed      | 
  |                                                        | locale                                           |                |
  +--------------------------------------------------------+--------------------------------------------------+----------------+
  | `PEP 539 <https://www.python.org/dev/peps/pep-0539/>`_ | A New C-API for Thread-Local Storage in CPython  | Won't Implement|
  +--------------------------------------------------------+--------------------------------------------------+----------------+
  | `PEP 540 <https://www.python.org/dev/peps/pep-0540/>`_ | UTF-8 mode                                       | Completed      |
  +--------------------------------------------------------+--------------------------------------------------+----------------+
  | `PEP 552 <https://www.python.org/dev/peps/pep-0552/>`_ | Deterministic pyc                                | Not Relevant   |
  +--------------------------------------------------------+--------------------------------------------------+----------------+
  | `PEP 553 <https://www.python.org/dev/peps/pep-0553/>`_ | Built-in breakpoint()                            | In Progress    |
  +--------------------------------------------------------+--------------------------------------------------+----------------+
  | `PEP 557 <https://www.python.org/dev/peps/pep-0557/>`_ | Data Classes                                     | In Progress    |
  +--------------------------------------------------------+--------------------------------------------------+----------------+
  | `PEP 560 <https://www.python.org/dev/peps/pep-0560/>`_ | Core support for typing module and generic types | In Progress    |
  +--------------------------------------------------------+--------------------------------------------------+----------------+
  | `PEP 562 <https://www.python.org/dev/peps/pep-0562/>`_ | Module __getattr__ and __dir__                   | Completed      |
  +--------------------------------------------------------+--------------------------------------------------+----------------+
  | `PEP 563 <https://www.python.org/dev/peps/pep-0563/>`_ | Postponed Evaluation of Annotations              | In Progress    |
  +--------------------------------------------------------+--------------------------------------------------+----------------+
  | `PEP 564 <https://www.python.org/dev/peps/pep-0564/>`_ | Time functions with nanosecond resolution        | Completed      |
  +--------------------------------------------------------+--------------------------------------------------+----------------+
  | `PEP 565 <https://www.python.org/dev/peps/pep-0565/>`_ | Show DeprecationWarning in __main__              | Won't Implement|
  +--------------------------------------------------------+--------------------------------------------------+----------------+
  | `PEP 567 <https://www.python.org/dev/peps/pep-0567/>`_ |  Context Variables                               | Won't Implement|
  +--------------------------------------------------------+--------------------------------------------------+----------------+


Other Language Changes:

  +----------------------------------------------------------------------------------------------------------+----------------+
  | async and await are now reserved keywords                                                                | Completed      |
  +----------------------------------------------------------------------------------------------------------+----------------+
  | dict objects must preserve insertion-order                                                               | In Progress    |
  +----------------------------------------------------------------------------------------------------------+----------------+
  | More than 255 arguments can now be passed to a function; a function can now have more than 255 parameters| Won't Implement|
  +----------------------------------------------------------------------------------------------------------+----------------+
  | bytes.fromhex() and bytearray.fromhex() now ignore all ASCII whitespace, not only spaces                 | In Progress    |
  +----------------------------------------------------------------------------------------------------------+----------------+
  | str, bytes, and bytearray gained support for the new isascii() method, which can be used to test if a    | In Progress    |
  | string or bytes contain only the ASCII characters                                                        |                |
  +----------------------------------------------------------------------------------------------------------+----------------+
  | ImportError now displays module name and module __file__ path whenfrom ... import ... fails              | Completed      |
  +----------------------------------------------------------------------------------------------------------+----------------+
  | Circular imports involving absolute imports with binding a submodule to a name are now supported         | Completed      |
  +----------------------------------------------------------------------------------------------------------+----------------+
  | object.__format__(x, '') is now equivalent to str(x) rather than format(str(self), '')                   | Completed      |
  +----------------------------------------------------------------------------------------------------------+----------------+
  | In order to better support dynamic creation of stack traces, types.TracebackType can now be instantiated | In Progress    |
  | from Python code, and the tb_next attribute on tracebacks is now writable                                |                |
  +----------------------------------------------------------------------------------------------------------+----------------+
  | When using the -m switch, sys.path[0] is now eagerly expanded to the full starting directory path, rather| Not Relevant   |
  | than being left as the empty directory (which allows imports from the current working directory at the   |                |
  | time when an import occurs)                                                                              |                |
  +----------------------------------------------------------------------------------------------------------+----------------+
  | The new -X importtime option or the PYTHONPROFILEIMPORTTIME environment variable can be used to show the | Not Relevant   | 
  | timing of each module import                                                                             |                |
  +----------------------------------------------------------------------------------------------------------+----------------+



Other Modules:

  +------------------------------------------------------------------------------------------------------------+----------------+
  | `asyncio <https://docs.python.org/3/whatsnew/3.7.html#asyncio>`_ asyncio (many, may need a separate ticket)| Completed      |
  +------------------------------------------------------------------------------------------------------------+----------------+
  | `gc <https://docs.python.org/3/whatsnew/3.7.html#gc>`_ New features include *gc.freeze()*,                 | Won't Implement| 
  | *gc.unfreeze()*, *gc-get_freeze_count*                                                                     |                |
  +------------------------------------------------------------------------------------------------------------+----------------+
  | `math <https://docs.python.org/3/whatsnew/3.7.html#math>`_ math.remainder() added to implement IEEE        | Completed      |
  | 754-style remainder                                                                                        |                |
  +------------------------------------------------------------------------------------------------------------+----------------+
  | `re <https://docs.python.org/3/whatsnew/3.7.html#re>`_ A number of tidy up features including better       | In Progress    |
  | support for splitting on empty strings and copy support for compiled expressions and match objects         |                |
  +------------------------------------------------------------------------------------------------------------+----------------+
  | `sys <https://docs.python.org/3/whatsnew/3.7.html#sys>`_ sys.breakpointhook() added.                       | In Progress    | 
  | sys.get(/set)_coroutine_origin_tracking_depth() added                                                      |                |
  +------------------------------------------------------------------------------------------------------------+----------------+
  | `time <https://docs.python.org/3/whatsnew/3.7.html#time>`_ Mostly updates to support nanosecond resolution |                | 
  | in PEP564, see above                                                                                       |                |  
  +------------------------------------------------------------------------------------------------------------+----------------+

