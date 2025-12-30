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

The following classes are provided:

* :class:`timedelta`
* :class:`timezone`
* :class:`datetime`

Python's classes `date()
<https://docs.python.org/3/library/datetime.html#date-objects>`_ and `time()
<https://docs.python.org/3/library/datetime.html#time-objects>`_
are mimicked with a class :class:`datetime` set to midnight, and a class
:class:`timedelta` in the range [0-24h).


:class:`timedelta` Objects
--------------------------

A :class:`timedelta` object represents a duration, the difference between two
dates or times. With respect to the Python module
`datetime <https://docs.python.org/3/library/datetime.html>`_,
this implementation is constrained as follows:

    * Minimum resolution is *1 nanosecond*, instead of *1 microsecond*.
    * Masimum delta spans over ±106751 days (±2\ :sup:`63` nanoseconds or
      ±292 years) instead of ±999999999 days.


Class attributes
^^^^^^^^^^^^^^^^

.. attribute:: timedelta.MINYEAR

   The year of :attr:`timedelta.min`, i.e.
   ``timedelta.min.tuple()[1] // (365 * 24 * 60 * 60 * 10**9) == -292``.


.. attribute:: timedelta.MAXYEAR

   The year of :attr:`timedelta.max`, i.e.
   ``timedelta.max.tuple()[1] // (365 * 24 * 60 * 60 * 10**9) == 292``.


.. attribute:: timedelta.min

   The most negative :class:`timedelta` object,
   ``timedelta(nanoseconds=-2**63)``.


.. attribute:: timedelta.max

   The most positive :class:`timedelta` object,
   ``timedelta(nanoseconds=2**63 - 1)``.


.. attribute:: timedelta.resolution

   The smallest possible difference between non-equal :class:`timedelta`
   objects, ``timedelta(nanoseconds=1)``.


.. attribute:: timedelta.nanoseconds

   The internal time delta representation as 64-bit integer.


Class methods
^^^^^^^^^^^^^

.. class:: timedelta(hours=0, minutes=0, seconds=0, days=0, weeks=0,
   milliseconds=0, microseconds=0, nanoseconds=0)

All arguments are optional and default to ``0``. Arguments may be integers
or floats, and may be positive or negative. Only nanoseconds are stored
internally. Arguments are converted to those units:

    * A microsecond is converted to 1000 nanoseconds.
    * A millisecond is converted to 10\ :sup:`6` nanoseconds.
    * A minute is converted to 60 seconds.
    * An hour is converted to 3600 seconds.
    * A week is converted to 7 days.

If no argument is a float, the conversion and normalization processes are
exact (no information is lost).


.. method:: timedelta.total_seconds()

   Return a float representing the total number of seconds contained in the duration.


.. method:: timedelta.__add__(other)

   Return a :class:`timedelta` which represents the sum of two durations.


.. method:: timedelta.__sub__(other)

   Return a :class:`timedelta` which represents the difference between
   two durations.


.. method:: timedelta.__mul__(other)

   Return a delta multiplied by an integer or float. The result is rounded to
   the nearest nanosecond using round-half-to-even.


.. method:: timedelta.__truediv__(other)

   When *other* is a float or an integer, returns a delta divided by *other*.
   The result is rounded to the nearest multiple of timedelta.resolution
   using round-half-to-even.

   When *other* is a delta, division of overall duration by interval unit
   *other*. Returns a float object.


.. method:: timedelta.__floordiv__(other)

   The floor is computed and the remainder (if any) is thrown away. When
   *other* is a delta, an integer is returned.


.. method:: timedelta.__mod__(other)

   The remainder is computed as a :class:`timedelta` object.


.. method:: timedelta.__divmod__(other)

   Computes the quotient and the remainder: ``q = td1.__floordiv__(td2)`` and
   ``r = td1.__mod__(td2)``.  ``q`` is an integer and ``r`` is a :class:`timedelta`
   object.


.. method:: timedelta.__neg__()

   Equivalent to ``td1.__mul__(-1)``.


.. method:: timedelta.__eq__(other)

   Equivalent to ``td1.total_seconds() == td2.total_seconds()``.


.. method:: timedelta.__le__(other)

   Equivalent to ``td1.total_seconds() <= td2.total_seconds()``.


.. method:: timedelta.__lt__(other)

   Equivalent to ``td1.total_seconds() < td2.total_seconds()``.


.. method:: timedelta.__ge__(other)

   Equivalent to ``td1.total_seconds() >= td2.total_seconds()``.


.. method:: timedelta.__gt__(other)

   Equivalent to ``td1.total_seconds() > td2.total_seconds()``.


.. method:: timedelta.__bool__()

   Return ``False`` when duration is ``0``.


.. method:: timedelta.__abs__()

   Return a positive delta.


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

   If the fractional part of :meth:`timedelta.total_seconds()` is not 0,
   ``.ffffff`` is appended.


.. method:: timedelta.tuple(sign_pos='')

   Return the tuple ``(sign, days, hours, minutes, seconds, nanoseconds)``,
   where ``sign`` is ``-`` if delta is negative, *sign_pos* otherwise.


Examples of usage
^^^^^^^^^^^^^^^^^

An example of normalization::

    import datetime.timedelta

    # Components of another_year add up to exactly 365 days
    year = timedelta(days=365)
    another_year = timedelta(weeks=40, days=84, hours=23, minutes=50, seconds=600)
    print(year == another_year)  # True
    print(year.total_seconds())  # 31536000.0


Examples of timedelta arithmetic::

    import datetime.timedelta

    year = timedelta(days=365)
    ten_years = year * 10
    print(ten_years)                    # 3650d 00:00:00
    nine_years = ten_years - year
    print(nine_years)                   # 3285d 00:00:00
    three_years = nine_years // 3
    print(three_years)                  # 1095d 00:00:00


:class:`timezone` Objects
-------------------------

The :class:`timezone` class represents a timezone defined by a fixed
offset from UTC. Define a subclass of :class:`timezone` to capture
information about a particular time zone.

An instance of :class:`timezone` can be passed to the constructors for
:class:`datetime`. The latter objects view their attributes as being in
local time, and the :class:`timezone` object supports methods revealing
offset of local time from UTC, the name of the time zone, and DST offset,
all relative to a date-time object passed to them.


Methods to customize
^^^^^^^^^^^^^^^^^^^^

A subclass of :class:`timezone` may need to override the following methods.
Exactly which methods are needed depends on the uses made of aware
:class:`datetime` objects. If in doubt, simply implement all of them.


.. method:: timezone.utcoffset(dt)

   Return offset of local time from UTC, as a :class:`timedelta` object
   that is positive east of UTC. If local time is west of UTC, this should
   be negative.

   This represents the *total* offset from UTC; for example, if a
   :class:`timezone` object represents both time zone and DST adjustments,
   :meth:`timezone.utcoffset` should return their sum. If the UTC offset
   isn’t known, return ``None``. Else the value returned must be a
   :class:`timedelta` object strictly between ``timedelta(hours=-24)`` and
   ``timedelta(hours=24)`` (the magnitude of the offset must be less than one
   day). Most implementations of :meth:`timezone.utcoffset` will probably
   look like one of these two:

        return CONSTANT                 # fixed-offset class
        return CONSTANT + self.dst(dt)  # daylight-aware class

   If :meth:`timezone.utcoffset` does not return ``None``, :meth:`timezone.dst`
   should not return None either.

   The default implementation of :meth:`timezone.utcoffset` returns the sum
   of time zone and DST adjustments, if available.

.. method:: timezone.dst(dt)

   Return the daylight saving time (DST) adjustment, as a :class:`timedelta`
   object or ``None`` if DST information isn’t known.

   Return ``timedelta(0)`` if DST is not in effect. If DST is in effect, return
   the offset as a :class:`timedelta` object (see :meth:`timezone.utcoffset`
   for details). Note that DST offset, if applicable, has already been added
   to the UTC offset returned by :meth:`timezone.utcoffset`, so there’s no
   need to consult :meth:`timezone.dst` unless you’re interested in obtaining
   DST info separately.

   Most implementations of :meth:`timezone.dst` will probably look like one
   of these two::

       def dst(self, dt):
           # a fixed-offset class:  doesn't account for DST
           return timedelta(0)

   or::

       def dst(self, dt):
           # Code to set dston and dstoff to the time zone's DST
           # transition times based on the input *dt*'s year, and
           # expressed in standard local time.

           dt_ = dt.replace(tzinfo=None)
           if dt_ >= dston and dt_ < dstoff:
               return timedelta(hours=1)
           else:
               return timedelta(0)

   The default implementation of :meth:`timezone.dst` returns ``None``.


.. method:: timezone.tzname(dt)

   Return the time zone name corresponding to the :class:`datetime` object
   *dt*, as a string. Nothing about string names is defined by the
   :class:`datetime` module, and there’s no requirement that it mean anything
   in particular. For example, “GMT”, “UTC”, “-500”, “-5:00”, “EDT”,
   “US/Eastern”, “America/New York” are all valid replies. Return ``None`` if
   a string name isn’t known. Note that this is a method rather than a fixed
   string primarily because some :class:`timezone` subclasses will wish to
   return different names depending on the specific value of *dt* passed,
   especially if the :class:`timezone` class is accounting for daylight time.

   The default implementation of :meth:`timezone.tzname` returns the fixed
   value specified when the :class:`timezone` instance is constructed.
   If *name* is not provided in the constructor, the name returned by
   ``tzname()`` is generated from the value of the ``offset`` as follows.
   If *offset* is ``timedelta(0)``, the name is “UTC”, otherwise it returns
   the string provided by :meth:`timezone.isoformat` method.

These methods are called by a :class:`datetime` object, in response to their
methods of the same names. A :class:`datetime` object passes *self* as *dt*
argument.


Class attributes
^^^^^^^^^^^^^^^^

.. attribute:: timezone.utc

   The UTC timezone, ``timezone(timedelta(0))``.


Class methods
^^^^^^^^^^^^^

.. class:: timezone(offset, name=None)

   The *offset* argument must be specified as a :class:`timedelta`
   object representing the difference between the local time and UTC.
   It must be strictly between ``timedelta(hours=-24)`` and
   ``timedelta(hours=24)``, otherwise :exc:`ValueError` is raised.

   The *name* argument is optional. If specified it must be a string
   that will be used as the value returned by the :meth:`datetime.tzname`
   method.


.. method:: timezone.isoformat(dt)

   Return a string in the format ``UTC±HH:MM``, where ``±`` is the sign of
   *offset* from UTC, ``HH`` and ``MM`` are two digits of offset's hours and
   offset's minutes respectively. If *offset* is ``timedelta(0)``, “UTC”
   is returned.

   If *utc* is ``False``, this method always returns ``±HH:MM``.

   *dt* is needed in determining the right offset; it can be ``None``.


Examples of usage
^^^^^^^^^^^^^^^^^

`Central European Time <https://en.wikipedia.org/wiki/Summer_time_in_Europe>`_
(CET), used in most parts of Europe and a few North African countries, is a
standard time which is 1 hour ahead of Coordinated Universal Time (UTC).
As of 2011, all member states of the European Union observe summer time;
those that during the winter use CET use Central European Summer Time (CEST)
(or: UTC+02:00, daylight saving time) in summer (from last Sunday of March
to last Sunday of October). ::

    import datetime

    class Cet(datetime.timezone):
        def __init__(self):
            super().__init__(datetime.timedelta(hours=1), "CET")

        def dst(self, dt):
            return datetime.timedelta(hours=1) if self.isdst(dt) else datetime.timedelta(0)

        def tzname(self, dt):
            return 'CEST' if self.isdst(dt) else 'CET'

        def isdst(self, dt):
            if dt is None:
                return False
            year, month, day, hour, minute, second, tz = dt.tuple()
            if not 2000 <= year < 2100:
                raise ValueError
            if 3 < month < 10:
                return True
            if month == 3:
                beg = 31 - (5*year//4 + 4) % 7 # last Sunday of March
                if day < beg: return False
                if day > beg: return True
                return hour >= 3
            if month == 10:
                end = 31 - (5*year//4 + 1) % 7 # last Sunday of October
                if day < end: return True
                if day > end: return False
                return hour < 3
            return False

    tz = Cet()
    print(tz.isoformat(datetime(2011, 1, 1))) # UTC+01:00
    print(tz.tzname   (datetime(2011, 1, 1))) # CET
    print(tz.isoformat(datetime(2011, 8, 1))) # UTC+02:00
    print(tz.tzname   (datetime(2011, 8, 1))) # CEST

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


:class:`datetime` Objects
-------------------------

A :class:`datetime` object is a single object containing all the information
for specifying an absolute date and time point.

:class:`datetime` assumes the current Gregorian calendar extended in both
directions, past and future. January 1 of year 1 is called day number 1,
January 2 of year 1 is called day number 2, and so on.

:class:`datetime` assumes there are exactly 3600*24 seconds in every day and
subject to adjustment via a :class:`timezone` object.


Constructors
^^^^^^^^^^^^

.. class:: datetime(self, year, month, day, hour=0, minute=0, second=0, nanosecond=0, tzinfo=None)

   The *year*, *month* and *day* arguments are required. *tzinfo* may be
   ``None``, or an instance of a :class:`timezone` class. The remaining
   arguments must be integers in the following ranges:

   * ``MINYEAR <= year <= MAXYEAR``,
   * ``1 <= month <= 12``,
   * ``1 <= day <= number of days in the given month and year``,
   * ``0 <= hour < 24``,
   * ``0 <= minute < 60``,
   * ``0 <= second < 60``,
   * ``0 <= nanosecond < 999_999_999``,

   If an argument outside those ranges is given, :exc:`ValueError` is raised.


.. function:: fromisoformat(date_string)

   Return a :class:`datetime` corresponding to a *date_string* in the format
   emitted by :meth:`datetime.isoformat`.

   Specifically, this function supports strings in the format::

       YYYY-MM-DD[*HH[:MM[:SS[.fff[fff]]]][+HH:MM[:SS[.ffffff]]]]

   where ``*`` can match any single character.


.. function:: fromordinal(n)

   Return the :class:`datetime` corresponding to the proleptic Gregorian
   ordinal, where January 1 of year 1 has ordinal 1. :exc:`ValueError` is
   raised unless ``1 <= ordinal <= datetime.max.toordinal()``. The hour,
   minute and second of the result are all 0, and *tzinfo* is ``None``.


.. function:: combine(date, time, tzinfo)

   Return a new :class:`datetime` object whose date components are equal to
   the given *date* object’s (see :meth:`datetime.date`), and whose time
   components are equal to the given *time* object’s (see
   :meth:`datetime.time`). If the *tzinfo* argument is provided, its value
   is used to set the *tzinfo* attribute of the result, otherwise the
   *tzinfo* attribute of the *date* argument is used.


Class attributes
^^^^^^^^^^^^^^^^

.. attribute:: datetime.MINYEAR

   The smallest year number allowed in a :class:`datetime` object.
   :attr:`datetime.MINYEAR` is ``1``.


.. attribute:: datetime.MAXYEAR

   The largest year number allowed in a :class:`datetime` object.
   :attr:`datetime.MAXYEAR` is ``9999``.


.. attribute:: datetime.EPOCH

   :class:`datetime` object representing the time epoch of 2000-01-01 00:00:00
   UTC (same as :mod:`time`'s epoch).


.. attribute:: datetime.timezone

   The object passed as the *tzinfo* argument to the :class:`datetime`
   constructor, or ``None`` if none was passed.


Class methods
^^^^^^^^^^^^^

.. method:: datetime.__add__(other)

   In the expression ``datetime2 = datetime1.__add__(timedelta)``, ``datetime2``
   is a duration of ``timedelta`` removed from ``datetime1``, moving forward
   in time if ``timedelta > 0``, or backward if ``timedelta < 0``. The result
   has the same :class:`timezone` attribute as the input ``datetime1``, and
   ``datetime2 - datetime1 == timedelta`` after.

   Note that no time zone adjustments are done even if the input is an aware
   object.


.. method:: datetime.__sub__(other)

   If *other* is an instance of :class:`timedelta`, the expression
   ``datetime2 = datetime1.__sub__(timedelta)`` computes the ``datetime2`` such
   that ``datetime2 + timedelta == datetime1``. As for addition, the result has
   the same :class:`timezone` attribute as the input ``datetime1``, and no time
   zone adjustments are done even if the input is aware.

   If *other* is an instance of :class:`datetime`, subtraction
   ``timedelta = datetime2.__sub__(datetime1)`` is defined only if both operands
   are naive, or if both are aware. If one is aware and the other is naive,
   :exc:`TypeError` is raised.

   If both are naive, or both are aware and have the same :class:`timezone`
   attribute, the :class:`timezone` attributes are ignored, and the result
   is a :class:`timedelta` object *t* such that ``datetime2 + t == datetime1``.
   No time zone adjustments are done in this case.

   If both are aware and have different :class:`timezone` attributes, ``a-b``
   acts as if *a* and *b* were first converted to naive UTC datetimes first.


.. method:: datetime.__lt__(other)

   Equivalent to ``dt1.toordinal() < dt2.toordinal()``.


.. method:: datetime.__le__(other)

   Equivalent to ``dt1.toordinal() <= dt2.toordinal()``.


.. method:: datetime.__eq__(other)

   Equivalent to ``dt1.toordinal() == dt2.toordinal()``.


.. method:: datetime.__ge__(other)

   Equivalent to ``dt1.toordinal() >= dt2.toordinal()``.


.. method:: datetime.__gt__(other)

   Equivalent to ``dt1.toordinal() > dt2.toordinal()``.


.. method:: datetime.utcoffset()

   If *tzinfo* is ``None``, returns ``None``, else returns a
   :class:`timedelta` object with magnitude less than one day.


.. method:: datetime.replace(year=None, month=None, day=None, hour=None, minute=None, second=None, nanosecond=None, tzinfo=True)

   Return a :class:`datetime` with the same attributes, except for those
   attributes given new values by whichever keyword arguments are specified.
   Note that ``tzinfo=None`` can be specified to create a naive
   :class:`datetime` from an aware :class:`datetime` with no conversion of
   date and time data.


.. method:: datetime.astimezone(tz)

   Return a :class:`datetime` object with new *tzinfo* attribute
   *tz*, adjusting the date and time data so the result is the same UTC time
   as *self*, but in *tz*’s local time. *self* must be aware.

   If you merely want to attach a :class:`timezone` object *tz* to a
   :class:`datetime` *dt* without adjustment of date and time data, use
   ``dt.replace(tzinfo=tz)``. If you merely want to remove the :class:`timezone`
   object from an aware :class:`datetime` *dt* without conversion of date and
   time data, use ``dt.replace(tzinfo=None)``.


.. method:: datetime.isoformat(sep='T')

   Return a string representing the date and time in ISO 8601 format
   ``YYYY-MM-DDTHH:MM:SS``. If :meth:`datetime.utcoffset` does not return
   ``None``, a string is appended, giving the UTC offset:
   ``YYYY-MM-DDTHH:MM:SS+HH:MM``.


.. method:: datetime.date()

   Return a :class:`datetime` instance whose date and time zone components
   are equal to the input object and time is set to midnight.


.. method:: datetime.time()

   Return a :class:`timedelta` instance whose time components are equal to
   the input object.


.. method:: datetime.toordinal()

   Return the proleptic Gregorian ordinal of the date.


.. method:: datetime.isoweekday()

   Return the day of the week as an integer, where Monday is 1 and Sunday
   is 7. For example, ``date(2002, 12, 4).isoweekday() == 3``, a Wednesday.


.. method:: datetime.tuple()

   Return the tuple ``(year, month, day, hour, minute, second, nanosecond, tzinfo)``.


Examples of usage
^^^^^^^^^^^^^^^^^

Examples of working with :class:`datetime` objects::

    from datetime import timedelta, timezone, datetime, fromisoformat

    print(datetime(2005, 7, 14, 12, 30))            # 2005-07-14 12:30:00
    dt = fromisoformat('2006-11-21 16:30+01:00')
    print(dt.add(timedelta(hours=23)))              # 2006-11-22 15:30:00+01:00
    tz1 = timezone(timedelta(hours=4, minutes=30))
    print(tz1)                                      # UTC+04:30
    dt = datetime(1900, 11, 21, 3, 30, tzinfo=tz1)
    print(dt)                                       # 1900-11-21 03:30:00+04:30
    print(dt.astimezone(timezone.utc))              # 1900-11-20 23:00:00+00:00
