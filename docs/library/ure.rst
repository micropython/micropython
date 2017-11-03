:mod:`ure` -- simple regular expressions
========================================

.. module:: ure
   :synopsis: regular expressions

|see_cpython_module| :mod:`python:re`.

This module implements regular expression operations. Regular expression
syntax supported is a subset of CPython ``re`` module (and actually is
a subset of POSIX extended regular expressions).

Supported operators are:

``'.'``
   Match any character.

``'[]'``
   Match set of characters. Individual characters and ranges are supported.

``'^'``

``'$'``

``'?'``

``'*'``

``'+'``

``'??'``

``'*?'``

``'+?'``

``'|'``

``'()'``
   Grouping. Each group is capturing (a substring it captures can be accessed
   with `match.group()` method).

Counted repetitions (``{m,n}``), more advanced assertions, named groups,
etc. are not supported.


Functions
---------

.. function:: compile(regex_str, [flags])

   Compile regular expression, return `regex <regex>` object.

.. function:: match(regex_str, string)

   Compile *regex_str* and match against *string*. Match always happens
   from starting position in a string.

.. function:: search(regex_str, string)

   Compile *regex_str* and search it in a *string*. Unlike `match`, this will search
   string for first position which matches regex (which still may be
   0 if regex is anchored).

.. data:: DEBUG

   Flag value, display debug information about compiled expression.
   (Availability depends on `MicroPython port`.)


.. _regex:

Regex objects
-------------

Compiled regular expression. Instances of this class are created using
`ure.compile()`.

.. method:: regex.match(string)
            regex.search(string)

   Similar to the module-level functions :meth:`match` and :meth:`search`.
   Using methods is (much) more efficient if the same regex is applied to
   multiple strings.

.. method:: regex.split(string, max_split=-1)

   Split a *string* using regex. If *max_split* is given, it specifies
   maximum number of splits to perform. Returns list of strings (there
   may be up to *max_split+1* elements if it's specified).

Match objects
-------------

Match objects as returned by `match()` and `search()` methods.

.. method:: match.group([index])

   Return matching (sub)string. *index* is 0 for entire match,
   1 and above for each capturing group. Only numeric groups are supported.
