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

``'[...]'``
   Match set of characters. Individual characters and ranges are supported,
   including negated sets (e.g. ``[^a-c]``).

``'^'``

``'$'``

``'?'``

``'*'``

``'+'``

``'??'``

``'*?'``

``'+?'``

``'|'``

``'(...)'``
   Grouping. Each group is capturing (a substring it captures can be accessed
   with `match.group()` method).

**NOT SUPPORTED**: Counted repetitions (``{m,n}``), more advanced assertions
(``\b``, ``\B``), named groups (``(?P<name>...)``), non-capturing groups
(``(?:...)``), etc.


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

.. function:: sub(regex_str, replace, string, count=0, flags=0)

   Compile *regex_str* and search for it in *string*, replacing all matches
   with *replace*, and returning the new string.

   *replace* can be a string or a function.  If it is a string then escape
   sequences of the form ``\<number>`` and ``\g<number>`` can be used to
   expand to the corresponding group (or an empty string for unmatched groups).
   If *replace* is a function then it must take a single argument (the match)
   and should return a replacement string.

   If *count* is specified and non-zero then substitution will stop after
   this many substitutions are made.  The *flags* argument is ignored.

   Note: availability of this function depends on `MicroPython port`.

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
            regex.sub(replace, string, count=0, flags=0)

   Similar to the module-level functions :meth:`match`, :meth:`search`
   and :meth:`sub`.
   Using methods is (much) more efficient if the same regex is applied to
   multiple strings.

.. method:: regex.split(string, max_split=-1)

   Split a *string* using regex. If *max_split* is given, it specifies
   maximum number of splits to perform. Returns list of strings (there
   may be up to *max_split+1* elements if it's specified).

Match objects
-------------

Match objects as returned by `match()` and `search()` methods, and passed
to the replacement function in `sub()`.

.. method:: match.group([index])

   Return matching (sub)string. *index* is 0 for entire match,
   1 and above for each capturing group. Only numeric groups are supported.

.. method:: match.groups()

   Return a tuple containing all the substrings of the groups of the match.

   Note: availability of this method depends on `MicroPython port`.

.. method:: match.start([index])
            match.end([index])

   Return the index in the original string of the start or end of the
   substring group that was matched.  *index* defaults to the entire
   group, otherwise it will select a group.

   Note: availability of these methods depends on `MicroPython port`.

.. method:: match.span([index])

   Returns the 2-tuple ``(match.start(index), match.end(index))``.

   Note: availability of this method depends on `MicroPython port`.
