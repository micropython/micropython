:mod:`ure` -- regular expressions
=================================

.. module:: ure
   :synopsis: regular expressions

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

Counted repetitions (``{m,n}``), more advanced assertions, named groups,
etc. are not supported.


Functions
---------

.. function:: compile(regex)

   Compile regular expression, return ``regex`` object.

.. function:: match(regex, string)

   Match ``regex`` against ``string``. Match always happens from starting
   position in a string.

.. function:: search(regex, string)

   Search ``regex`` in a ``string``. Unlike ``match``, this will search
   string for first position which matches regex (which still may be
   0 if regex is anchored).

.. data:: DEBUG

   Flag value, display debug information about compiled expression.


Regex objects
-------------

Compiled regular expression. Instances of this class are created using
``ure.compile()``.

.. method:: regex.match(string)

.. method:: regex.search(string)

.. method:: regex.split(string, max_split=-1)


Match objects
-------------

Match objects as returned by ``match()`` and ``search()`` methods.

.. method:: match.group([index])

   Only numeric groups are supported.
