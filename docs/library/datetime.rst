:mod:`datetime` -- basic date and time types
============================================

.. module:: datetime
   :synopsis: basic date and time types

|see_cpython_module| :mod:`python:datetime`.

This module supplies classes for manipulating dates, times, and deltas.

:class:`datetime` objects may be categorized as “aware” or “naive”
depending on whether or not they include timezone information.
An aware object can locate itself relative to other aware objects. An
*aware* object represents a specific moment in time that is not open to
interpretation.

A *naive* object does not contain enough information to unambiguously
locate itself relative to other :class:`datetime` objects. Whether a naive
object represents Coordinated Universal Time (UTC), local time, or time
in some other timezone is purely up to the program, just like it is up
to the program whether a particular number represents metres, miles, or
mass. Naive objects are easy to understand and to work with, at the cost
of ignoring some aspects of reality.

For applications requiring aware objects, :class:`datetime` objects have
an optional time zone information attribute, *tzinfo*, that can be set to
an instance of a :class:`timezone` class. These objects capture
information about the offset from UTC time and the time zone name.


Constants
---------

The datetime module exports the following constants:

.. data:: MINYEAR

    The smallest year number allowed in a :class:`date` or :class:`datetime`
    object. :const:`MINYEAR` is 1.


.. data:: MAXYEAR

    The largest year number allowed in a :class:`date` or :class:`datetime`
    object. :const:`MAXYEAR` is 9 999.


Available Types
---------------

The following classes are provided:

* :class:`timedelta`: a duration expressing the difference between two date,
  time, or datetime instances to microsecond resolution.
* :class:`tzinfo`: an abstract base class for time zone information objects.
  These are used by the :class:`!datetime` and :class:`!time` classes to
  provide a customizable notion of time adjustment (for example, to account for
  time zone and/or daylight saving time).
* :class:`timezone`: a class that implements the tzinfo abstract base class as
  a fixed offset from the UTC.
* :class:`date`: an idealized naive date, assuming the current Gregorian
  calendar always was, and always will be, in effect.
* :class:`time`: an idealized time, independent of any particular day, assuming
  that every day has exactly 24*60*60 seconds. (There is no notion of “leap
  seconds” here.)
* :class:`datetime`: a combination of a date and a time.

Objects of these types are immutable. They are also *hashable*, meaning that
they can be used as dictionary keys.


:class:`timedelta` Objects
-----------------------------------------------------------------------------

A :class:`!timedelta` object represents a duration, the difference between two
dates or times. With respect to the Python module `datetime
<https://docs.python.org/3/library/datetime.html>`_, this implementation
differs in:

    * ``__repr__()`` produces a different, yet compatible, output.
    * Delta is stored as a signed bignum integer of microseconds.


Constructors
^^^^^^^^^^^^

.. class:: timedelta(days=0, seconds=0, microseconds=0, milliseconds=0, minutes=0, hours=0, weeks=0)

All arguments are optional and default to ``0``. Arguments may be integers or
floats, and may be positive or negative. Arguments are "merged" and normalized
into microseconds according to the following conversions:

    * A week is converted to 7 days.
    * A day is converted to 24 hours.
    * An hour is converted to 60 minutes.
    * A minute is converted to 60 seconds.
    * A second is converted to 1000 milliseconds.
    * A millisecond is converted to 1000 microseconds.

If no argument is a float, the conversion and normalization processes are
exact (no information is lost).

Note that normalization of negative values may be surprising at first. For
example::

    >>> from datetime import timedelta
    >>> td = timedelta(hours=-1)
    >>> days, hours, minutes, seconds, microseconds = td.tuple()
    >>> days, hours, minutes, seconds, microseconds
    (-1, 23, 0, 0, 0)


Class attributes
^^^^^^^^^^^^^^^^

.. attribute:: timedelta.min

   The most negative :class:`timedelta` object, ``timedelta(-999999999)``.


.. attribute:: timedelta.max

   The most positive :class:`timedelta` object, ``timedelta(days=999_999_999,
   hours=23, minutes=59, seconds=59, microseconds=999_999)``.


.. attribute:: timedelta.resolution

   The smallest possible difference between non-equal :class:`timedelta`
   objects, ``timedelta(microseconds=1)``.


.. attribute:: timedelta.days
.. attribute:: timedelta.seconds
.. attribute:: timedelta.microseconds

   Days (between -999 999 999 and 999 999 999 inclusive), seconds (between 0
   and 86 399 inclusive) and microseconds (between 0 and 999 999 inclusive)
   representing the time delta. If you need two or more of them, consider using
   :meth:`~timedelta.tuple` instead.


Operations
^^^^^^^^^^

The class support a number of operators.


Comparison operators
""""""""""""""""""""

+-------------------------------+---------------------------------------------------------------------------------------+
| Operation                     | Result                                                                                |
+===============================+=======================================================================================+
| ``td``                        | True when *td.microseconds* is not 0.                                                 |
+-------------------------------+---------------------------------------------------------------------------------------+
| ``td1 < td2``                 | True when *td1.microseconds* is less than, less or equal to, equal to, not equal to,  |
+-------------------------------+ greater or equal to, greater than *td2.microseconds*. False otherwise.                |
| ``td1 <= td2``                |                                                                                       |
+-------------------------------+                                                                                       |
| ``td1 == td2``                |                                                                                       |
+-------------------------------+                                                                                       |
| ``td1 != td2``                |                                                                                       |
+-------------------------------+                                                                                       |
| ``td1 >= td2``                |                                                                                       |
+-------------------------------+                                                                                       |
| ``td1 > td2``                 |                                                                                       |
+-------------------------------+---------------------------------------------------------------------------------------+


Arithmetic operators
""""""""""""""""""""

+-------------------------------+---------------------------------------------------------------------------------------+
| Operation                     | Result                                                                                |
+===============================+=======================================================================================+
| ``+td1``                      | A :class:`timedelta` with the same value.                                             |
+-------------------------------+---------------------------------------------------------------------------------------+
| ``-td1``                      | A :class:`timedelta` equivalent to *-1 * td1*.                                        |
+-------------------------------+---------------------------------------------------------------------------------------+
| ``td3 = td1 + td2``           | A :class:`timedelta`, sum of *t1* and *t2*. Afterwards, *td1 == td3 - td2*.           |
+-------------------------------+---------------------------------------------------------------------------------------+
| ``td3 = td1 - td2``           | A :class:`timedelta`, difference of *t1* and *t2*. Same as *td1 + (-td2)*.            |
+-------------------------------+---------------------------------------------------------------------------------------+
| | ``td2 = i * td1``           | A :class:`timedelta`, *i* times greater than *td1*. Afterwards, *td1 // i == td2*.    |
| | ``td2 = td1 * i``           |                                                                                       |
+-------------------------------+---------------------------------------------------------------------------------------+
| ``td2 = f * td1``             | A :class:`timedelta` rounded to the nearest microsecond using round-half-to-even.     |
+-------------------------------+---------------------------------------------------------------------------------------+
| ``f = td2 / td1``             | A :class:`float`, division of overall duration *td2* by interval unit *td1*.          |
+-------------------------------+---------------------------------------------------------------------------------------+
| | ``td2 = td1 / i``           | A :class:`timedelta`, division of a float or an int. The result is rounded to the     |
| | ``td2 = td1 / f``           | nearest microsecond using round-half-to-even.                                         |
+-------------------------------+---------------------------------------------------------------------------------------+
| ``td2 = td1 // i``            | A :class:`timedelta`, floor of the integer division (remainder is discarded).         |
+-------------------------------+---------------------------------------------------------------------------------------+
| ``td3 = td2 // td1``          | An integer, floor of the integer division (remainder is discarded).                   |
+-------------------------------+---------------------------------------------------------------------------------------+
| ``td3 = td2 % td1``           | A :class:`timedelta`, remainder of the integer division.                              |
+-------------------------------+---------------------------------------------------------------------------------------+
| ``q, r = divmod(td1, td2)``   | Computes the quotient and the remainder: ``q = t1 // t2`` and ``r = t1 % t2``.        |
+-------------------------------+---------------------------------------------------------------------------------------+
| ``abs(td)``                   | A :class:`timedelta` equivalent to *+td* when ``td.days >= 0`` and to *-td* otherwise.|
+-------------------------------+---------------------------------------------------------------------------------------+


Other operators
"""""""""""""""

+-------------------------------+---------------------------------------------------------------------------------------+
| Operation                     | Result                                                                                |
+===============================+=======================================================================================+
| ``str(td)``                   | A string in the form ``[[-]D day[s], ][H]H:MM:SS[.UUUUUU]``.                          |
+-------------------------------+---------------------------------------------------------------------------------------+
| ``repr(td)``                  | A string representation of a constructor call with canonical attribute values.        |
+-------------------------------+---------------------------------------------------------------------------------------+
| ``hash(td)``                  | The hash number for dictionary keys.                                                  |
+-------------------------------+---------------------------------------------------------------------------------------+


Class methods
^^^^^^^^^^^^^

.. method:: timedelta.total_seconds()

   Return a float representing the total number of seconds contained in the duration.


.. method:: timedelta.isoformat()

   This method mimics Python's `isoformat()
   <https://docs.python.org/3/library/datetime.html#datetime.time.isoformat>`_
   for *time* objects by returning a string in the format ``HH:MM:SS``, where
   ``HH``, ``MM``, and ``SS`` are two digits of the time delta's hours,
   minutes and seconds, respectively, since midnight.  This is only if value
   is within the range [0-24h).

   For other values, it returns the format ``±Dd HH:MM:SS``, where ``±`` is
   the sign of the delta and ``D`` its number of days. This is *not* ISO
   compliant, but provides a complete representation.

   If the fractional part of :meth:`~timedelta.total_seconds()` is not 0,
   ``.ffffff`` is appended.


.. method:: timedelta.tuple(sign_pos='')

   Return the tuple ``(sign, days, hours, minutes, seconds, microseconds)``,
   where ``sign`` is ``-`` if delta is negative, *sign_pos* otherwise.


Examples of usage
^^^^^^^^^^^^^^^^^

An example of normalization::

    from datetime import timedelta

    # Components of another_year add up to exactly 365 days
    year = timedelta(days=365)
    another_year = timedelta(weeks=40, days=84, hours=23, minutes=50, seconds=600)
    print(year == another_year)  # True
    print(year.total_seconds())  # 31536000.0


Examples of timedelta arithmetic::

    from datetime import timedelta

    year = timedelta(days=365)
    ten_years = year * 10
    print(ten_years)                    # 3650d 00:00:00
    nine_years = ten_years - year
    print(nine_years)                   # 3285d 00:00:00
    three_years = nine_years // 3
    print(three_years)                  # 1095d 00:00:00


:class:`tzinfo` Objects
-----------------------------------------------------------------------------

This is an abstract base class, meaning that this class should not be
instantiated directly.  Define a subclass of :class:`!tzinfo` to capture
information about a particular time zone.

An instance of (a concrete subclass of) :class:`!tzinfo` can be passed to the
constructors for :class:`datetime` and :class:`time` objects. The latter
objects view their attributes as being in local time, and the :class:`!tzinfo`
object supports methods revealing offset of local time from UTC, the name of
the time zone, and DST offset, all relative to a date or time object passed to
them.

The :mod:`datetime` module provides :class:`timezone`, a simple concrete
subclass of :class:`!tzinfo` which can represent timezones with fixed offset
from UTC such as UTC.

A concrete subclass of :class:`!tzinfo` may need to implement the following
methods. Exactly which methods are needed depends on the uses made of aware
:mod:`datetime` objects. If in doubt, simply implement all of them.


.. method:: tzinfo.tzname(dt)

   Return the time zone name corresponding to the :class:`datetime` object
   *dt*, as a string. Return ``None`` if a string name isn't known.

   If the :class:`tzinfo` class is accounting for daylight time, this method
   will likely return different names depending on the specific value of *dt*
   passed.

   The default implementation raises :exc:`NotImplementedError`.


.. method:: tzinfo.utcoffset(dt)

   Return offset of local time from UTC, as a :class:`timedelta` object that is
   positive east of UTC. If local time is west of UTC, this should be negative.

   This represents the *total* offset from UTC; for example, if a
   :class:`tzinfo` object represents both time zone and DST adjustments,
   :meth:`utcoffset` should return their sum. If the UTC offset isn't known,
   return ``None``. Else the value returned must be a :class:`timedelta` object
   strictly between ``timedelta(hours=-24)`` and ``timedelta(hours=24)``. Most
   implementations of :meth:`utcoffset` will probably look like one of these
   two::

      return CONSTANT                 # fixed-offset class
      return CONSTANT + self.dst(dt)  # daylight-aware class

   If :meth:`utcoffset` does not return ``None``, :meth:`dst` should not return
   ``None`` either.

   The default implementation raises :exc:`NotImplementedError`.


.. method:: tzinfo.dst(dt)

   Return the daylight saving time (DST) adjustment, as a :class:`timedelta`
   object or ``None`` if DST information isn't known.

   Return ``timedelta(0)`` if DST is not in effect. If DST is in effect, return
   the offset as a :class:`timedelta` object (see :meth:`utcoffset` for
   details). Note that DST offset, if applicable, has already been added to the
   UTC offset returned by :meth:`utcoffset`, so there's no need to consult
   :meth:`dst` unless you're interested in obtaining DST info separately. For
   example, :meth:`datetime.timetuple` calls its :attr:`~datetime.tzinfo`
   attribute's :meth:`dst` method to determine how the :attr:`tm_isdst` flag
   should be set.

   Most implementations of :meth:`dst` will probably look like one of these
   two::

      def dst(self, dt):
          # a fixed-offset class:  doesn't account for DST
          return timedelta(0)

   or::

      def dst(self, dt):
          # Code to set dston and dstoff to the time zone's DST
          # transition times based on the input dt.year, and expressed
          # in standard local time.

          if dston <= dt.replace(tzinfo=None) < dstoff:
              return timedelta(hours=1)
          else:
              return timedelta(0)

   The default implementation raises :exc:`NotImplementedError`.


These methods are called by a :class:`datetime` or :class:`time` object, in
response to their methods of the same names. A :class:`datetime` object passes
itself as the argument, and a :class:`time` object passes ``None`` as the
argument. A :class:`tzinfo` subclass's methods should therefore be prepared to
accept a *dt* argument of ``None``, or of class :class:`datetime`.

When ``None`` is passed, it's up to the class designer to decide the best
response. For example, returning ``None`` is appropriate if the class wishes to
say that time objects don't participate in the :class:`tzinfo` protocols. It
may be more useful for ``utcoffset(None)`` to return the standard UTC offset,
as there is no other convention for discovering the standard offset.

When a :class:`datetime` object is passed in response to a :class:`datetime`
method, ``dt.tzinfo`` is the same object as *self*. :class:`tzinfo` methods can
rely on this, unless user code calls :class:`tzinfo` methods directly. The
intent is that the :class:`tzinfo` methods interpret *dt* as being in local
time, and not need worry about objects in other timezones.

There is one more :class:`tzinfo` method that a subclass may wish to override:

.. method:: tzinfo.fromutc(dt)

   This is called from the default :class:`datetime.astimezone()`
   implementation. When called from that, ``dt.tzinfo`` is *self*, and *dt*'s
   date and time data are to be viewed as expressing a UTC time. The purpose of
   :meth:`fromutc` is to adjust the date and time data, returning an equivalent
   datetime in *self*'s local time.

   Most :class:`tzinfo` subclasses should be able to inherit the default
   :meth:`fromutc` implementation without problems. It's strong enough to
   handle fixed-offset time zones, and time zones accounting for both standard
   and daylight time, and the latter even if the DST transition times differ in
   different years.

   The default implementations of :meth:`astimezone` and :meth:`fromutc` may
   not produce the result you want if the result is one of the hours straddling
   the moment the standard offset changes. Such cases must be computed and
   handled by a user's implementation via :attr:`datetime.fold`.


Example of usage
^^^^^^^^^^^^^^^^

`Central European Time <https://en.wikipedia.org/wiki/Summer_time_in_Europe>`_
(CET), used in most parts of Europe and a few North African countries, is a
standard time which is 1 hour ahead of Coordinated Universal Time (UTC).
As of 2011, all member states of the European Union observe summer time;
those that during the winter use CET use Central European Summer Time (CEST)
(or: UTC+02:00, daylight saving time) in summer (from last Sunday of March
to last Sunday of October). ::

    class Cet(tzinfo):
        def utcoffset(self, dt):
            h = 2 if self.isdst(dt)[0] else 1
            return timedelta(hours=h)

        def dst(self, dt):
            h = 1 if self.isdst(dt)[0] else 0
            return timedelta(hours=h)

        def tzname(self, dt):
            return "CEST" if self.isdst(dt)[0] else "CET"

        def fromutc(self, dt):
            assert dt.tzinfo is self
            isdst, fold = self.isdst(dt, utc=True)
            h = 2 if isdst else 1
            dt += timedelta(hours=h)
            dt = dt.replace(fold=fold)
            return dt

        def isdst(self, dt, utc=False):
            if dt is None:
                return False, None

            year = dt.year
            if not 2000 <= year < 2100:
                # Formulas below are valid in the range [2000; 2100)
                raise ValueError

            hour = 1 if utc else 3
            day = 31 - (5 * year // 4 + 4) % 7  # last Sunday of March
            beg = datetime(year, 3, day, hour)
            day = 31 - (5 * year // 4 + 1) % 7  # last Sunday of October
            end = datetime(year, 10, day, hour)

            dt = dt.replace(tzinfo=None)
            if utc:
                fold = 1 if end <= dt < end + timedelta(hours=1) else 0
            else:
                fold = dt.fold
            isdst = beg <= dt < end
            return isdst, fold


The two formulas above are a simplification of the general
`Gauss's algorithm (disparate variation)
<https://en.wikipedia.org/wiki/Determination_of_the_day_of_the_week#Disparate_variation>`_
for calculating the week day of any given date::

    def week_day (year, month, day, verbose=False):
        if month <= 2:
            raise ValueError

        Y = year
        y = Y%100
        c = Y//100
        m = month - 2
        d = day
        W = ('Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat')

        k1 = d + int(2.6*m - 0.2) - 507*c//4
        k  = k1 % 7
        w  = (5*year//4 + k) % 7
        print(f'w(year) = (5*year//4 + {k}) % 7   # {100*c} <= year < {(c+1)*100}')

        if verbose:
            print()
            print(f'year = {year}, month = {month}, day = {day}')
            print( 'w(y) = (d + int(2.6*m - 0.2) + y + y//4 + c//4 - 2*c) % 7')
            print( 'w(y) = (d + int(2.6*m - 0.2) + 5*y//4 - 7*c//4) % 7')
            print()
            print( 'w(year) = (d + int(2.6*m - 0.2) + 5*year//4 - 5*c*100//4 - 7*c//4) % 7')
            print( 'w(year) = (d + int(2.6*m - 0.2) + 5*year//4 - 507*c//4) % 7')
            print(f'w(year) = ({d} + int(2.6*{m} - 0.2) + 5*year//4 - 507*{c}//4) % 7')
            print(f'w(year) = (5*year//4 + ({k1}) % 7) % 7')
            print(f'w(year) = (5*year//4 + {k}) % 7')
            print(f'w({year}) = {w} ({W[w]})')

which produces the following output for March and October::

    >>> week_day(2000, 3, 31)
    w(year) = (5*year//4 + 4) % 7   # 2000 <= year < 2100
    >>> week_day(2000, 10, 31)
    w(year) = (5*year//4 + 1) % 7   # 2000 <= year < 2100


:class:`timezone` Objects
-----------------------------------------------------------------------------

The :class:`!timezone` class is a subclass of :class:`tzinfo`, each instance of
which represents a timezone defined by a fixed offset from UTC.

An instance of :class:`!timezone` can be passed to the constructors for
:class:`datetime` and :class:`time`. The latter objects view their attributes
as being in local time, and the :class:`!timezone` object supports methods
revealing offset of local time from UTC, the name of the time zone, and DST
offset, all relative to a date-time object passed to them.


Constructors
^^^^^^^^^^^^

.. class:: timezone(offset, name=None)

  The *offset* argument must be specified as a :class:`timedelta` object
  representing the difference between the local time and UTC. It must be
  strictly between ``-timedelta(hours=24)`` and ``timedelta(hours=24)``,
  otherwise :exc:`ValueError` is raised.

  The *name* argument is optional. If specified it must be a string.


Class attributes
^^^^^^^^^^^^^^^^

.. attribute:: timezone.utc

   The UTC timezone, ``timezone(timedelta(0))``.


Class methods
^^^^^^^^^^^^^

.. method:: timezone.utcoffset(dt)

  Return the fixed value specified when the :class:`timezone` instance is
  constructed.

  The *dt* argument is ignored. The return value is a :class:`timedelta`
  instance equal to the difference between the local time and UTC.

.. method:: timezone.dst(dt)

  Always returns ``None``.

.. method:: timezone.tzname(dt)

  Return the fixed value specified when the :class:`timezone` instance is
  constructed.

  If *name* is not provided in the constructor, the name returned by
  ``tzname(dt)`` is generated from the value of the ``offset`` as a string in
  the format ``UTC±HH:MM``.

.. method:: timezone.fromutc(dt)

  Return ``dt + offset``. The *dt* argument must be an aware
  :class:`datetime` instance, with ``tzinfo`` set to ``self``.


:class:`date` Objects
-----------------------------------------------------------------------------

A :class:`!date` object represents a date (year, month and day). :class:`!date`
assumes the current Gregorian calendar extended in both directions, past and
future. January 1 of year 1 is called day number 1, January 2 of year 1 is
called day number 2, and so on.

With respect to the Python module `datetime
<https://docs.python.org/3/library/datetime.html>`_, this implementation
differs in:

    * ``__repr__()`` produces a different, yet compatible, output.
    * ``ctime()``, ``strftime()`` and ``__format__()`` are not implemented.
    * ``fromisocalendar()`` and ``isocalendar()`` are not implemented.


Constructors
^^^^^^^^^^^^

.. class:: date(year, month, day)

   All arguments are required. Arguments must be integers, in the following
   ranges:

   * ``MINYEAR <= year <= MAXYEAR``
   * ``1 <= month <= 12``
   * ``1 <= day <= number of days in the given month and year``

   If an argument outside those ranges is given, :exc:`ValueError` is raised.


.. classmethod:: date.fromtimestamp(timestamp)

   Return the local date corresponding to the POSIX timestamp, such as is
   returned by :func:`time.time`.


.. classmethod:: date.today()

   Return the current local date. This is equivalent to
   ``date.fromtimestamp(time.time())``.


.. classmethod:: date.fromordinal(ordinal)

   Return a :class:`date` corresponding to the proleptic Gregorian ordinal,
   where January 1 of year 1 has ordinal 1.


.. classmethod:: date.fromisoformat(date_string)

   Return a :class:`date` corresponding to a *date_string* given in the format
   ``YYYY-MM-DD``.


Class attributes
^^^^^^^^^^^^^^^^

.. attribute:: date.min
.. attribute:: date.max

   The earliest (``date(MINYEAR, 1, 1)``) and the latest (``date(MAXYEAR, 12,
   31)``) representable :class:`date`.


.. attribute:: date.resolution

   The smallest possible difference between non-equal date objects,
   ``timedelta(days=1)``.

.. attribute:: date.year
.. attribute:: date.month
.. attribute:: date.day

   Year (between :const:`MINYEAR` and :const:`MAXYEAR` inclusive), month
   (between 1 and 12 inclusive) and day (between 1 and the number of days in
   the given month of the given year) representing the date. If you need two or
   more of them, consider using :meth:`~date.tuple` instead.


Operations
^^^^^^^^^^

The class support a number of operators.


Comparison operators
""""""""""""""""""""

+---------------------+---------------------------------------------------------------------------------+
| Operation           | Result                                                                          |
+=====================+=================================================================================+
| ``d1 < d2``         | True when *t1.toordinal()* is less than, less or equal to, greater or equal to, |
+---------------------+ greater than *t2.toordinal()*. False otherwise.                                 |
| ``d1 <= d2``        |                                                                                 |
+---------------------+                                                                                 |
| ``d1 >= d2``        |                                                                                 |
+---------------------+                                                                                 |
| ``d1 > d2``         |                                                                                 |
+---------------------+---------------------------------------------------------------------------------+
| ``d1 == d2``        | Same as above, but it returns :const:`False` (:const:`True`) even when *d2* is  |
+---------------------+ not a :class:`date`.                                                            |
| ``d1 != d2``        |                                                                                 |
+---------------------+---------------------------------------------------------------------------------+



Arithmetic operators
""""""""""""""""""""

+---------------------+---------------------------------------------------------------------------------+
| Operation           | Result                                                                          |
+=====================+=================================================================================+
| ``d2 = d1 + td``    | *d2* is moved forward (if *td.days > 0*) or backward (if *td.days < 0*) in      |
|                     | time. Afterward *d2 - d1 == td.days*. *td.seconds* and *td.microseconds* are    |
|                     | ignored.                                                                        |
+---------------------+---------------------------------------------------------------------------------+
| ``d2 = d1 - td``    | *d2* is moved backward (if *td.days > 0*) or forward (if *td.days < 0*) in      |
|                     | time. Afterward *d2 + td.days == d1*. *td.seconds* and *td.microseconds* are    |
|                     | ignored.                                                                        |
+---------------------+---------------------------------------------------------------------------------+
| ``td = d2 - d1``    | A :class:`timedelta`, such that *d1 + td == d2*. *td.seconds* and               |
|                     | *td.microseconds* are always 0.                                                 |
+---------------------+---------------------------------------------------------------------------------+


Other operators
"""""""""""""""

+---------------------+---------------------------------------------------------------------------------+
| Operation           | Result                                                                          |
+=====================+=================================================================================+
| ``str(d)``          | A string in the form returned by *d.isoformat()* (see :meth:`~date.isoformat`). |
+---------------------+---------------------------------------------------------------------------------+
| ``repr(d)``         | A string representation of a constructor call with canonical attribute values.  |
+---------------------+---------------------------------------------------------------------------------+
| ``hash(d)``         | The hash number for dictionary keys.                                            |
+---------------------+---------------------------------------------------------------------------------+


Class methods
^^^^^^^^^^^^^

.. method:: date.toordinal()

   Return the proleptic Gregorian ordinal of the date, where January 1 of year
   1 has ordinal 1. For any :class:`date` object *d*,
   ``date.fromordinal(d.toordinal()) == d``.


.. method:: date.timetuple()

   Return a tuple such as returned by :func:`time.localtime`. The hours,
   minutes and seconds are 0, and the DST flag is -1.


.. method:: date.replace(year=None, month=None, day=None)

   Return a :class:`date` with the same value, except for those parameters
   given new values by whichever keyword arguments are specified.


.. method:: date.weekday()

   Return the day of the week as an integer, where Monday is 0 and Sunday is 6.


.. method:: date.isoweekday()

   Return the day of the week as an integer, where Monday is 1 and Sunday is 7.


.. method:: date.isoformat()

   Return a string representing the date in ISO 8601 format, ``YYYY-MM-DD``.
   This is the inverse of :meth:`~date.fromisoformat`.


.. method:: date.tuple()

   Return the tuple of attributes ``(year, month, day)``.

   This method should be preferred when two or more attributes are required
   because it is less computationally intesive than getting each attribute
   separately.


:class:`time` Objects
-----------------------------------------------------------------------------

A :class:`!time` object represents a (local) time of day, independent of any
particular day, and subject to adjustment via a :class:`tzinfo` object.

With respect to the Python module `datetime
<https://docs.python.org/3/library/datetime.html>`_, this implementation
differs in:

    * ``__repr__()`` produces a different, yet compatible, output.
    * ``__format__()`` is not implemented.
    * ``strftime()`` and ``__format__()`` are not implemented.


Constructors
^^^^^^^^^^^^

.. class:: time(hour=0, minute=0, second=0, microsecond=0, tzinfo=None, *, fold=0)

   All arguments are optional. *tzinfo* may be ``None``, or an instance of a
   :class:`tzinfo` subclass. The remaining arguments must be integers in the
   following ranges:

   * ``0 <= hour < 24``,
   * ``0 <= minute < 60``,
   * ``0 <= second < 60``,
   * ``0 <= microsecond < 1_000_000``,
   * ``fold in [0, 1]`` (see :attr:`~time.fold`).

   If an argument outside those ranges is given, :exc:`ValueError` is raised.


.. classmethod:: time.fromisoformat(time_string)

   Return a :class:`time` corresponding to a *time_string* in one of the
   formats emitted by :meth:`~time.isoformat`. Specifically, this function
   supports strings in the format:

   .. code-block:: none

      HH[:MM[:SS[.fff[fff]]]][+HH:MM[:SS[.ffffff]]]


Class attributes
^^^^^^^^^^^^^^^^

.. attribute:: time.min
.. attribute:: time.max

   The earliest (``time(0, 0, 0, 0)``) and the latest (``time(23, 59, 59,
   999_999)``) representable :class:`time`.


.. attribute:: time.resolution

   The smallest possible difference between non-equal :class:`time` objects,
   ``timedelta(microseconds=1)``, although note that arithmetic on
   :class:`time` objects is not supported.


.. attribute:: time.hour
.. attribute:: time.minute
.. attribute:: time.second
.. attribute:: time.microsecond

   Hour (in ``range(24)``), minute (in ``range(60)``), second (in
   ``range(60)``), microsecond (in ``range(1_000_000)``) representing the time.
   If you need two or more of them, consider using :meth:`~time.tuple` instead.


.. attribute:: time.tzinfo

   The object passed as the tzinfo argument to the :class:`time` constructor,
   or ``None`` if none was passed. See also :meth:`~time.tuple`.


.. attribute:: time.fold

   In ``[0, 1]``. Used to disambiguate wall times during a repeated interval.
   The value 0 (1) represents the earlier (later) of the two moments with the
   same wall time representation.

   A repeated interval occurs when clocks are rolled back at the end of
   daylight saving time (DST), typically in Autumn. Consider a timezone whose
   DST ends the minute after 1:59 on the last Sunday of October. There's an
   hour that can't be spelled unambiguously in local wall time: the last hour
   of daylight time. The local wall clock leaps from 1:59 (daylight time) back
   to 1:00 (standard time) again. Local times of the form 1:MM are ambiguous.

   Applications that can't bear wall-time ambiguities should explicitly check
   the value *fold* attribute or avoid using hybrid :class:`tzinfo` subclasses.

   There are no ambiguities when using :class:`timezone` or any other
   **fixed-offset** :class:`tzinfo` subclass.

   See also :meth:`~time.tuple`.


Operations
^^^^^^^^^^

The class support a number of operators.


Comparison operators
""""""""""""""""""""

+---------------------+---------------------------------------------------------------------------------+
| Operation           | Result                                                                          |
+=====================+=================================================================================+
| ``t1 < t2``         | Both *t1* and *t2* must be naive or aware datetime objects. When both are       |
|                     | naive, or both are aware and have the same :attr:`~time.tzinfo`, operands       |
+---------------------+ return: true when *t1.total_seconds()* is less or greater than                  |
| ``t1 <= t2``        | *t2.total_seconds()*; when equal, true is returned for operators ``<=`` and     |
|                     | ``>=``. Otherwise, they evaluate to :const:`False`.                             |
+---------------------+                                                                                 |
| ``t1 >= t2``        | When both comparands are aware and their :attr:`~time.tzinfo` differs, they are |
|                     | first adjusted by subtracting their UTC offsets (obtained from                  |
+---------------------+ :meth:`~time.utcoffset`). The comparison then proceeds as above.                |
| ``t1 > t2``         |                                                                                 |
|                     | When one comparand is naive and the other is aware, :exc:`TypeError` is raised. |
+---------------------+---------------------------------------------------------------------------------+
| ``t1 == t2``        | Same as above, but *total_seconds()* is compared for (in)equality.              |
|                     |                                                                                 |
+---------------------+ Plus, when one comparand is naive and the other is aware, operations evaluate   |
| ``t1 != t2``        | to :const:`False` for ``==`` and to :const:`True` for ``!=``. No exception is   |
|                     | raised.                                                                         |
+---------------------+---------------------------------------------------------------------------------+


Arithmetic operators
""""""""""""""""""""

No arithmetic operations are allowed.


Other operators
"""""""""""""""

+---------------------+---------------------------------------------------------------------------------+
| Operation           | Result                                                                          |
+=====================+=================================================================================+
| ``str(t)``          | A string in the form returned by *t.isoformat()* (see :meth:`~time.isoformat`). |
+---------------------+---------------------------------------------------------------------------------+
| ``repr(t)``         | A string representation of a constructor call with canonical attribute values.  |
+---------------------+---------------------------------------------------------------------------------+
| ``hash(t)``         | The hash number for dictionary keys.                                            |
+---------------------+---------------------------------------------------------------------------------+


Class methods
^^^^^^^^^^^^^

.. method:: time.replace(hour=None, minute=None, second=None, \
   microsecond=None, tzinfo=True, *, fold=0)

   Return a :class:`time` with the same value, except for those attributes
   given new values by whichever keyword arguments are specified. Note that
   ``tzinfo=None`` can be specified to create a naive :class:`time` from an
   aware :class:`time`, without conversion of the time data.


.. method:: time.isoformat(timespec="auto")

   Return a string representing the time in ISO 8601 format, one of:

   - ``HH:MM:SS.ffffff``, if :attr:`~time.microsecond` is not 0
   - ``HH:MM:SS``, if :attr:`~time.microsecond` is 0
   - ``HH:MM:SS.ffffff+HH:MM[:SS[.ffffff]]``, if :meth:`~time.utcoffset` does
     not return ``None``
   - ``HH:MM:SS+HH:MM[:SS[.ffffff]]``, if :attr:`~time.microsecond` is 0 and
     :meth:`~time.utcoffset` does not return ``None``

   The optional argument *timespec* specifies the number of additional
   components of the time to include (the default is ``"auto"``). It can be one
   of the following:

   - ``"auto"``: same as ``"seconds"`` if :attr:`~time.microsecond` is 0, same
     as ``"microseconds"`` otherwise.
   - ``"hours"``: include the :attr:`~time.hour` in the two-digit ``HH``
     format.
   - ``"minutes"``: include :attr:`~time.hour` and :attr:`~time.minute` in
     ``HH:MM`` format.
   - ``"seconds"``: include :attr:`~time.hour`, :attr:`~time.minute`, and
     :attr:`~time.second` in ``HH:MM:SS`` format.
   - ``"milliseconds"``: include full time, but truncate fractional second part
     to milliseconds. ``HH:MM:SS.sss`` format.
   - ``"microseconds"``: include full time in ``HH:MM:SS.ffffff`` format.

   .. note::

      Excluded time components are truncated, not rounded.


.. method:: time.utcoffset()

   If :attr:`~time.tzinfo` is ``None``, returns ``None``, else returns
   ``self.tzinfo.utcoffset(None)``.


.. method:: time.dst()

   If :attr:`~time.tzinfo` is ``None``, returns ``None``, else returns
   ``self.tzinfo.dst(None)``.


.. method:: time.tzname()

   If :attr:`~time.tzinfo` is ``None``, returns ``None``, else returns
   ``self.tzinfo.tzname(None)``.


.. method:: time.tuple()

   Return the tuple of attributes ``(hour, minute, second, microsecond, tzinfo,
   fold)``.

   This method should be preferred when two or more attributes are required
   because it is less computationally intesive than getting each attribute
   separately.


:class:`datetime` Objects
-----------------------------------------------------------------------------

A :class:`!datetime` object is a single object containing all the information
for specifying an absolute date and time point.

:class:`!datetime` assumes the current Gregorian calendar extended in both
directions, past and future. January 1 of year 1 is called day number 1,
January 2 of year 1 is called day number 2, and so on.

:class:`!datetime` assumes there are exactly 3600*24 seconds in every day and
subject to adjustment via a :class:`timezone` object.

With respect to the Python module `datetime
<https://docs.python.org/3/library/datetime.html>`_, this implementation
differs in:

    * ``__repr__()`` produces a different, yet compatible, output.
    * ``today()`` is not provided. Use :meth:`~datetime.now` instead.
    * ``utcnow()`` is not provided. Use ``datetime.now(timezone.utc)``
      instead.
    * ``utcfromtimestamp()`` is not implemented. Use
      ``datetime.fromtimestamp(timestamp, tz=timezone.utc)`` instead.
    * ``utctimetuple()`` is not implemented. Use
      :meth:`~datetime.timetuple` on an aware object made with
      ``datetime.replace(tzinfo=timezone.utc)``.
    * ``ctime()``, ``strftime()`` and ``__format__()`` are not implemented.


Constructors
^^^^^^^^^^^^

.. class:: datetime(year, month, day, hour=0, minute=0, second=0, microsecond=0, tzinfo=None, *, fold=0)

   The *year*, *month* and *day* arguments are required. *tzinfo* may be
   ``None``, or an instance of a :class:`timezone` class. The remaining
   arguments must be integers in the following ranges:

   * ``MINYEAR <= year <= MAXYEAR``,
   * ``1 <= month <= 12``,
   * ``1 <= day <= d``, where ``d`` is the number of days in the given month and year,
   * ``0 <= hour < 24``,
   * ``0 <= minute < 60``,
   * ``0 <= second < 60``,
   * ``0 <= microsecond < 1_000_000``,
   * ``fold in [0, 1]`` (see :attr:`~datetime.fold`).

   If an argument outside those ranges is given, :exc:`ValueError` is raised.


.. classmethod:: datetime.now(tz=None)

   Return the current local date and time.

   If *tz* is not ``None``, it must be an instance of a :class:`tzinfo`
   subclass, and the current date and time are converted to *tz*’s time zone.


.. classmethod:: datetime.fromtimestamp(timestamp, tz=None)

   Return the local date and time corresponding to the timestamp defined by
   :attr:`~datetime.EPOCH`, such as is returned by :func:`time.time`.

   *tz* must be an instance of a :class:`tzinfo` subclass, and the timestamp is
   converted to *tz*’s time zone. ``tz=None`` is not supported.

   .. note::

      If *tz* is naive, the CPython algorithm relies on platform's
      :func:`time.localtime` to perform the conversion which must properly
      handle daylight saving time for the timezone of interest in order to
      compute meaningful dates and times. Micropython board ports do not
      support timezones.


.. classmethod:: datetime.fromordinal(n)

   Return the :class:`datetime` corresponding to the proleptic Gregorian
   ordinal, where January 1 of year 1 has ordinal 1. :exc:`ValueError` is
   raised unless ``1 <= ordinal <= datetime.max.toordinal()``. The hour,
   minute, second and microsecond of the result are all 0, and
   :attr:`~datetime.tzinfo` is ``None``.


.. classmethod:: datetime.fromisoformat(date_string)

   Return a :class:`datetime` corresponding to a *date_string* in the format
   emitted by :meth:`~datetime.isoformat`. Specifically, this function supports
   strings in the format:

   .. code-block:: none

       YYYY-MM-DD[*HH[:MM[:SS[.fff[fff]]]][+HH:MM[:SS[.ffffff]]]]

   where ``*`` can match any single character.


.. classmethod:: datetime.combine(date, time, tzinfo)

   Return a new :class:`datetime` object whose date components are equal to the
   given *date* object’s (see :meth:`~datetime.date`), and whose time
   components are equal to the given *time* object’s (see
   :meth:`~datetime.time`). If the *tzinfo* argument is provided, its value is
   used to set the *tzinfo* attribute of the result, otherwise the *tzinfo*
   attribute of the *time* argument is used.


Class attributes
^^^^^^^^^^^^^^^^

.. attribute:: datetime.EPOCH

   :class:`datetime` object representing the same time epoch as :mod:`time`'s,
   typically 2000-01-01 00:00:00 UTC for embedded ports and 1970-01-01 00:00:00
   UTC for Unix port.


.. attribute:: datetime.year
.. attribute:: datetime.month
.. attribute:: datetime.day
.. attribute:: datetime.hour
.. attribute:: datetime.minute
.. attribute:: datetime.second
.. attribute:: datetime.microsecond

   Year (between :const:`MINYEAR` and :const:`MAXYEAR` inclusive), month
   (between 1 and 12 inclusive), day (between 1 and the number of days in the
   given month of the given year), hour (in ``range(24)``), minute (in
   ``range(60)``), second (in ``range(60)``) and microsecond (in
   ``range(1_000_000)`` representing the date and time. If you need two or more
   of them, consider using :meth:`~datetime.tuple` instead.


.. attribute:: datetime.tzinfo

   The object passed as the *tzinfo* argument to the
   :class:`~datetime.datetime` constructor, or ``None`` if none was passed. See
   also :meth:`~datetime.tuple`.


.. attribute:: datetime.fold

   In ``[0, 1]``. Used to disambiguate wall times during a repeated interval.
   The value 0 (1) represents the earlier (later) of the two moments with the
   same wall time representation. See :attr:`time.fold` for a thorough
   explanation.

   See also :meth:`~datetime.tuple`.


Operations
^^^^^^^^^^

The class support a number of operators.


Comparison operators
""""""""""""""""""""

+---------------------+---------------------------------------------------------------------------------+
| Operation           | Result                                                                          |
+=====================+=================================================================================+
| ``dt1 < dt2``       | Both *dt1* and *dt2* must be naive or aware datetime objects. When both are     |
|                     | naive, or both are aware and have the same :attr:`~datetime.tzinfo`, operands   |
+---------------------+ return: true when *dt1.date()* is less or greater than *dt2.date()*; when       |
| ``dt1 <= dt2``      | equal, true is returned when *dt1.time()* is less or greater than *dt2.time()*; |
|                     | when equal, true is returned for operators ``<=`` and ``>=``. Otherwise, they   |
+---------------------+ evaluate to :const:`False`.                                                     |
| ``dt1 >= dt2``      |                                                                                 |
|                     | When both comparands are aware and their :attr:`~datetime.tzinfo` differs, they |
+---------------------+ are first adjusted by subtracting their UTC offsets (obtained from              |
| ``dt1 > dt2``       | :meth:`~datetime.utcoffset`). The comparison then proceeds as above.            |
|                     |                                                                                 |
|                     | When one comparand is naive and the other is aware, :exc:`TypeError` is raised. |
+---------------------+---------------------------------------------------------------------------------+
| ``dt1 == dt2``      | Same as above, but *date()* and *time()* are compared for (in)equality.         |
|                     |                                                                                 |
+---------------------+ Plus, when one comparand is naive and the other is aware, operations evaluate   |
| ``dt1 != dt2``      | to :const:`False` for ``==`` and to :const:`True` for ``!=``. No exception is   |
|                     | raised.                                                                         |
+---------------------+---------------------------------------------------------------------------------+


Arithmetic operators
""""""""""""""""""""

+---------------------+---------------------------------------------------------------------------------+
| Operation           | Result                                                                          |
+=====================+=================================================================================+
| ``dt2 = dt1 + td``  | A :class:`datetime`, moved forward in time if ``td.days`` > 0, or backward if   |
|                     | ``td.days`` < 0. Afterwards, *dt2 - dt1 == td*. *dt2* has the same              |
|                     | :attr:`~datetime.tzinfo` attribute as *dt1*.                                    |
+---------------------+---------------------------------------------------------------------------------+
| ``dt2 = dt1 - td``  | A :class:`datetime`, moved backward or forward in time. Same as *dt1 + (-td)*.  |
+---------------------+---------------------------------------------------------------------------------+
| ``td = dt2 - dt1``  | A :class:`timedelta`, such that *dt1 + td == dt2*.                              |
|                     |                                                                                 |
|                     | Both *dt1* and *dt2* must be naive or aware datetime objects. When both are     |
|                     | naive, or both are aware and have the same :attr:`~datetime.tzinfo`, *tzinfo*   |
|                     | attributes are ignored and no time zone adjustments are done.                   |
|                     |                                                                                 |
|                     | If both are aware and have different *tzinfo* attributes, *dt1* and *dt2* are   |
|                     | first  adjusted by subtracting their UTC offsets (obtained from                 |
|                     | :meth:`~datetime.utcoffset`).                                                   |
|                     |                                                                                 |
|                     | If one is aware and the other is naive, :exc:`TypeError` is raised.             |
+---------------------+---------------------------------------------------------------------------------+


Other operators
"""""""""""""""

+---------------------+---------------------------------------------------------------------------------+
| Operation           | Result                                                                          |
+=====================+=================================================================================+
| ``str(dt)``         | A string in the form returned by *dt.isoformat(" ")*                            |
|                     | (see :meth:`~datetime.isoformat`).                                              |
+---------------------+---------------------------------------------------------------------------------+
| ``repr(dt)``        | A string representation of a constructor call with canonical attribute values.  |
+---------------------+---------------------------------------------------------------------------------+
| ``hash(dt)``        | The hash number for dictionary keys.                                            |
+---------------------+---------------------------------------------------------------------------------+


Class methods
^^^^^^^^^^^^^

.. method:: datetime.date()

   Return :class:`date` object with same year, month and day.


.. method:: datetime.time()

   Return :class:`time` object with same hour, minute, second, microsecond and
   fold. :attr:`~datetime.tzinfo` is ``None``. See also method
   :meth:`~datetime.timetz`.


.. method:: datetime.timetz()

   Return :class:`time` object with same hour, minute, second, microsecond,
   fold, and tzinfo attributes. See also method :meth:`~datetime.time`.


.. method:: datetime.replace(year=None, month=None, day=None, hour=None, \
   minute=None, second=None, nanosecond=None, tzinfo=True, *, fold=0)

   Return a :class:`datetime` with the same attributes, except for those
   attributes given new values by whichever keyword arguments are specified.
   Note that ``tzinfo=None`` can be specified to create a naive
   :class:`datetime` from an aware :class:`datetime` with no conversion of date
   and time data.


.. method:: datetime.astimezone(tz=None)

   Return a :class:`datetime` object with new :attr:`~datetime.tzinfo`
   attribute *tz*, adjusting the date and time data so the result is the same
   UTC time as *self*, but in *tz*’s local time.

   *self* must be aware.

   If provided, *tz* must be an instance of a :class:`tzinfo` subclass. Its
   :meth:`tzinfo.utcoffset` and :meth:`tzinfo.dst` methods must not return
   :const:`None`. If *tz* is not provided, the system local timezone is assumed
   for the target timezone: the :attr:`~datetime.tzinfo` will be set to an
   instance of :class:`timezone` with the offset obtained from the OS.

   If you merely want to attach a :class:`timezone` object *tz* to a
   :class:`datetime` *dt* without adjustment of date and time data, use
   ``dt.replace(tzinfo=tz)``. If you merely want to remove the
   :class:`timezone` object from an aware :class:`datetime` *dt* without
   conversion of date and time data, use ``dt.replace(tzinfo=None)``.

   .. note::

      If *self* is naive, the CPython algorithm relies on platform's
      :func:`time.localtime` to perform the conversion which must properly
      handle daylight saving time for the timezone of interest in order to
      compute meaningful dates and times. Micropython board ports do not
      support timezones.


.. method:: datetime.utcoffset()

   If :attr:`~datetime.tzinfo` is ``None``, returns ``None``, else returns a
   :class:`timedelta` object with magnitude less than one day.


.. method:: datetime.dst()

   If :attr:`~datetime.tzinfo` is ``None``, returns ``None``, else returns
   ``self.tzinfo.dst(self)``.


.. method:: datetime.tzname()

   If :attr:`~datetime.tzinfo` is ``None``, returns ``None``, else returns
   ``self.tzinfo.tzname(self)``.


.. method:: datetime.timetuple()

   Return a tuple such as returned by :func:`time.localtime`.

   .. note::

      If *self* is naive, the algorithm relies on platform's
      :func:`time.localtime` to perform the conversion which must properly
      handle daylight saving time for the timezone of interest in order to
      compute meaningful dates and times.


.. method:: datetime.toordinal()

   Return the proleptic Gregorian ordinal of the date. The same as
   ``self.date().toordinal()``.


.. method:: datetime.timestamp()

   Return a timestamp since :attr:`EPOCH` and corresponding to the
   :class:`datetime` instance. The return value is a :class:`float` similar to
   that returned by :func:`time.time`.

   For aware :class:`datetime` instances, the return value is computed
   as::

      (dt - datetime.EPOCH).total_seconds()

   Naive :class:`datetime` instances are not supported.

   .. note::

      If *self* is naive, the CPython algorithm relies on platform's
      :func:`time.localtime` to perform the conversion which must properly
      handle daylight saving time for the timezone of interest in order to
      compute meaningful dates and times. Micropython board ports do not
      support timezones.


.. method:: datetime.weekday()

   Return the day of the week as an integer, where Monday is 0 and Sunday is 6.
   The same as ``self.date().weekday()``. See also
   :meth:`~datetime.isoweekday`.


.. method:: datetime.isoweekday()

   Return the day of the week as an integer, where Monday is 1 and Sunday is 7.
   The same as ``self.date().isoweekday()``. See also
   :meth:`~datetiem.weekday`.


.. method:: datetime.isoformat(sep="T", timespec="auto")

   Return a string representing the date and time in ISO 8601 format:

   - ``YYYY-MM-DDTHH:MM:SS.ffffff``,
     if :attr:`~datetiem.microsecond` is not 0
   - ``YYYY-MM-DDTHH:MM:SS``,
     if :attr:`~datetiem.microsecond` is 0

   If :meth:`~datetiem.utcoffset` does not return ``None``, a string is
   appended, giving the UTC offset:

   - ``YYYY-MM-DDTHH:MM:SS.ffffff+HH:MM[:SS[.ffffff]]``,
     if :attr:`~datetiem.microsecond` is not 0
   - ``YYYY-MM-DDTHH:MM:SS+HH:MM[:SS[.ffffff]]``,
     if :attr:`~datetiem.microsecond` is 0

   The optional argument *sep* (default ``"T"``) is a one-character separator,
   placed between the date and time portions of the result.

   The optional argument *timespec* specifies the number of additional
   components of the time to include (the default is ``"auto"``). It can be one
   of the following:

   - ``"auto"``: same as ``"seconds"`` if :attr:`~datetiem.microsecond` is 0,
     same as ``"microseconds"`` otherwise.
   - ``"hours"``: include the :attr:`~datetiem.hour` in the two-digit ``HH``
     format.
   - ``"minutes"``: include :attr:`~datetiem.hour` and :attr:`~datetiem.minute`
     in ``HH:MM`` format.
   - ``"seconds"``: include :attr:`~datetiem.hour`, :attr:`~datetiem.minute`,
     and :attr:`~datetiem.second` in ``HH:MM:SS`` format.
   - ``"milliseconds"``: include full time, but truncate fractional second part
     to milliseconds. ``HH:MM:SS.sss`` format.
   - ``"microseconds"``: include full time in ``HH:MM:SS.ffffff`` format.

   .. note::

      Excluded time components are truncated, not rounded.


.. method:: datetime.tuple()

   Return the tuple of attributes ``(year, month, day, hour, minute, second,
   microsecond, tzinfo, fold)``.

   This method should be preferred when two or more attributes are required
   because it is less computationally intesive than getting each attribute
   separately.


Examples of usage
^^^^^^^^^^^^^^^^^

Examples of working with :class:`datetime` objects::

    from datetime import timedelta, timezone, datetime

    print(datetime(2005, 7, 14, 12, 30))            # 2005-07-14 12:30:00
    dt = datetime.fromisoformat('2006-11-21 16:30+01:00')
    print(dt + timedelta(hours=23))                 # 2006-11-22 15:30:00+01:00
    tz1 = timezone(timedelta(hours=4, minutes=30))
    print(tz1)                                      # UTC+04:30
    dt = datetime(1900, 11, 21, 3, 30, tzinfo=tz1)
    print(dt)                                       # 1900-11-21 03:30:00+04:30
    print(dt.astimezone(timezone.utc))              # 1900-11-20 23:00:00+00:00


Resource Optimization
-----------------------------------------------------------------------------

Class :class:`datetime` has been implemented to be lightly dependent from
:class:`date` and :class:`time` classes. Several source code lines can be
deleted from ``datetime.py`` as to save precious resources, when needed.

If :class:`date` class is not required, delete:

    - class ``date``
    - methods ``datetime.combine`` and ``datetime.date``

If :class:`time` class is not required, delete:

    - class ``time``
    - methods ``datetime.combine``, ``datetime.time`` and ``datetime.timetz``

If :meth:`datetime.fromisoformat` is not required, delete:

    - functions ``_iso2d`` and ``_iso2t``
    - methods ``date.fromisoformat``, ``time.fromisoformat`` and
      ``datetime.fromisoformat``

If :meth:`datetime.isoformat` is not required, delete:

    - functions ``_d2iso`` and ``_t2iso``
    - methods ``date.isoformat``, ``time.isoformat`` and ``datetime.isoformat``
