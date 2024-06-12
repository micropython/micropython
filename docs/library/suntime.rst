:mod:`suntime` -- sunrise and sunset time
=========================================

.. module:: suntime
      :synopsis: sunrise and sunset time

This library provides an approximated calculation of sunrise and sunset time.
It contains two classes, :class:`Sundatetime` and :class:`Suntime`, which
differ from the input and output data types.


Class :class:`Sundatetime`
--------------------------

The following class makes use of module :mod:`datetime` for expressing input
date and output time.

.. class:: Sundatetime(latitude, longitude, altitude=0)

   Arguments *latitude* and *longitude* are floats representing the
   coordinates of a place on Earth. *altitude* is an integer number for
   observer's elevation in meters.

.. method:: Sundatetime.calc_sunrise_sunset(date)

   Calculate the sunrise and sunset for the given date. *date* must be an
   *aware* `datetime.datetime` object in the range [2000-01-01; 2100-01-01).
   Time information is ignored, whereas time zone ``tzinfo`` is used to
   provide meaningful output. The results are cached in :data:`sunrise` and
   :data:`sunset` .

.. method:: Sundatetime.is_daytime(now)
            Sundatetime.is_nighttime(now)

   Argument *now* is an *aware* `datetime.datetime` object representing a
   point in time. A boolean value is returned whether Sun is above/below the
   horizon or not, ``None`` when data are meaningless.

.. method:: Sundatetime.is_sunrise(now)
            Sundatetime.is_sunset(now)

   Argument *now* is an *aware* `datetime.datetime` object representing a
   point in time. A boolean value is returned whether *now* matches
   sunrise/sunset time or not, ``None`` when data are meaningless.

The following instance variables are accessible:

.. data:: Sundatetime.latitude
          Sundatetime.longitude

   Float numbers for coordinates on Earth.

.. data:: Sundatetime.altitude

   Elevation in meters for observations above the sea horizon. It corrects
   for both apparent dip and terrestrial refraction.

.. data:: Sundatetime.sunrise
          Sundatetime.sunset

   They hold ``None`` when an instance is created, an *aware*
   `datetime.datetime` after :meth:`calc_sunrise_sunset` is called.

.. note::
   :data:`sunrise` may occur before 00:00 and :data:`sunset` after 23:59
   on calculated *date*.  See :ref:`unexpected-results`.


Class :class:`Suntime`
----------------------

The following class makes use of plain integers for expressing input date and
output time.

.. class:: Suntime(latitude, longitude, altitude=0, timezone=0)

   Arguments *latitude* and *longitude* are floats representing the
   coordinates of a place on Earth. *altitude* is an integer number for
   observer's elevation in meters. *timezone* is an integer holding the
   timezone offset from UTC in minutes. The results are cached in
   :data:`sunrise` and :data:`sunset`.

.. method:: Suntime.calc_sunrise_sunset(year, month, day, dst=0)

   Calculate the sunrise and sunset for the given year, month and day.
   *year* must be in the range [2000; 2100). *dst* is an integer holding the
   offset in minute (usually 60) that accounts for Daylight Saving Time.

.. method:: Suntime.is_daytime(now)
            Suntime.is_nighttime(now)

   Argument *now* is an integer holding the number of minutes since midnight.
   A boolean value is returned whether Sun is above/below the horizon or not,
   ``None`` when data are meaningless.

.. method:: Suntime.is_sunrise(now)
            Suntime.is_sunset(now)

   Argument *now* is an an integer holding the number of minutes since midnight.
   A boolean value is returned whether *now* matches sunrise/sunset time or not,
   ``None`` when data are meaningless.

The following instance variables are accessible:

.. data:: Suntime.latitude
          Suntime.longitude

   Float numbers for coordinates on Earth.

.. data:: Suntime.altitude

   Elevation in meters for observations above the sea horizon. It corrects
   for both apparent dip and terrestrial refraction.

.. data:: Suntime.sunrise
          Suntime.sunset

   It holds ``None`` when an instance is created, an integer for the
   difference in minutes since 00:00 after :meth:`calc_sunrise_sunset`
   is called.

.. note::
   ``sunrise`` may be negative and ``sunset`` may be greater than 1439
   (24 hours). See :ref:`unexpected-results`.

.. _unexpected-results:

Unexpected results
------------------

:class:`Sundatetime` may return unexpected results: :data:`Sundatetime.sunrise`
may come before 00:00 and/or :data:`Sundatetime.sunset` may come after 23:59.
Similarly, :class:`Suntime` may return a negative :data:`Suntime.sunrise`
and/or a :data:`Suntime.sunset` greater or equal to 1440 (24 hours).

Assuming ``date`` is the current date and ``now`` is the current time,
the conditions which may lead to unexpected results are:

* *incorrect time zone*: *date*'s time zone is not consistent with provided
  *longitude*. Suitable values for *timezone* and *dst* should be provided.
  See example #3 (Novosibirsk) below.

* *Sun is up all day*: close to the poles, Sun never sets in summer/winter
  time. For this dates, ``is_daytime()`` holds true for the whole ``date``.
  Note that the following holds true: ``sunrise ≤ now < sunset``.

* *Sun is down all day*: close to the poles, Sun never raises in summer/winter
  time. For this dates, ``is_nighttime()`` holds true for the whole ``date``.
  Note that the following holds false: ``sunrise < sunset``.

* *Sun sets after midnight*: close to the poles, Sun may sets after
  23:59. In this case, ``is_daytime()`` and ``is_nighttime()`` behave as
  expected. The following condition is true: ``sunrise ≤ now ≤ 23:59 < sunset``.


Examples of usage
-----------------

A typical use case is::

    import datetime, suntime, time

    class Cet(datetime.timezone):
        # See `datetime` documentation

    # initialization
    CET = Cet()
    Rome = suntime.Sundatetime(42.5966460, 12.4360233)
    Rome.calc_sunrise_sunset(datetime.datetime(2000, 1, 1, tzinfo=CET))

    # main loop (every minute or more)
    now = datetime.datetime(*time.localtime()[:5], tzinfo=CET)
    if (now.date() > Rome.sunset.date()):
        Rome.calc_sunrise_sunset(now)
    Rome.is_daytime(now)

The following script shows sunrise and sunset time for several places and dates::

    # place: latitude    longitude
    pl1 = ( 42.5966460,  12.4360233) # Rome
    pl2 = ( 51.1627938,-122.9593616) # Vancouver
    pl3 = (-33.9252192,  18.4240762) # CapeTown
    pl4 = ( 55.1574890,  82.8547661) # Novosibirsk
    pl5 = ( 78.6560170,  16.3447384) # Pyramiden
    pl6 = pl5
    pl7 = (-77.7817838, 166.4561470) # McMurdo
    pl8 = pl7

    # date: YY   MM  DD  sunrise  sunset
    dt1 = (2000,  1,  1) # 7:37   16:49 - https://www.timeanddate.com/sun/italy/rome?month=1&year=2000
    dt2 = (2014, 10,  3) # 7:15   18:46 - https://www.timeanddate.com/sun/canada/vancouver?month=10&year=2014
    dt3 = (2016, 12, 21) # 5:32   19:57 - https://www.timeanddate.com/sun/south-africa/cape-town?month=12&year=2016
    dt4 = (2021,  4, 24) # 6:04   20:50 - https://www.timeanddate.com/sun/russia/novosibirsk?month=4&year=2021
    dt5 = (2040,  8, 25) #  up all day  - https://www.timeanddate.com/sun/@2729216?month=8&year=2033
    dt6 = (2040,  8, 26) #        00:09
                         # 1:45   23:41 - https://www.timeanddate.com/sun/@2729216?month=8&year=2040
    dt7 = (2033,  8, 10) # down all day - https://www.timeanddate.com/sun/antarctica/mcmurdo?month=8&year=2033
    dt8 = (2033, 10, 21) # 3:00   24:13 - https://www.timeanddate.com/sun/antarctica/mcmurdo?month=10&year=2033

    # timezone offsets and DSTs (in hours)
    tz1 = ( 1, 0)
    tz2 = (-8, 1)
    tz3 = ( 2, 0)
    tz4 = ( 0, 0) # wrong; it generates negative hour because actual timezone is (7, 0)
    tz5 = ( 1, 1)
    tz6 = ( 1, 1)
    tz7 = (13,-1)
    tz8 = (13, 0)

The following snippet of code makes use of class :class:`Sundatetime`::

    from suntime import Sundatetime
    from datetime import datetime, timedelta, timezone

    class Tz(timezone):
        def __init__(self, hours, dst=0):
            super().__init__(timedelta(hours=hours))
            self._dst = dst

        def dst(self, dt):
            return timedelta(hours=self._dst) if self.isdst(dt) else timedelta(0)

        def isdst(self, dt):
            return self._dst != 0

    print('Rome:')
    sd1 = Sundatetime(*pl1)
    sd1.calc_sunrise_sunset(datetime(*dt1, tzinfo=Tz(*tz1)))
    print('>', sd1.sunrise) # 2000-01-01 07:40:00+01:00
    print('>', sd1.sunset ) # 2000-01-01 16:47:00+01:00

    print('Vancouver:')
    sd2 = Sundatetime(*pl2)
    sd2.calc_sunrise_sunset(datetime(*dt2, tzinfo=Tz(*tz2)))
    print('>', sd2.sunrise) # 2014-10-03 07:16:00-08:00
    print('>', sd2.sunset ) # 2014-10-03 18:46:00-08:00

    print('Cape Town:')
    sd3 = Sundatetime(*pl3)
    sd3.calc_sunrise_sunset(datetime(*dt3, tzinfo=Tz(*tz3)))
    print('>', sd3.sunrise) # 2016-12-21 05:32:00+02:00
    print('>', sd3.sunset ) # 2016-12-21 19:57:00+02:00

    print('Novosibirsk:')
    sd4 = Sundatetime(*pl4)
    sd4.calc_sunrise_sunset(datetime(*dt4, tzinfo=Tz(*tz4)))
    print('>', sd4.sunrise) # 2021-04-23 23:04:00+00:00
    print('>', sd4.sunset ) # 2021-04-24 13:49:00+00:00

    print('Pyramiden:')
    sd5 = Sundatetime(*pl5)
    sd5.calc_sunrise_sunset(datetime(*dt5, tzinfo=Tz(*tz5)))
    print('>', sd5.sunrise) # 2040-08-24 12:57:00+02:00
    print('>', sd5.sunset ) # 2040-08-26 12:57:00+02:00

    print('Pyramiden:')
    sd6 = Sundatetime(*pl6)
    sd6.calc_sunrise_sunset(datetime(*dt6, tzinfo=Tz(*tz6)))
    print('>', sd6.sunrise) # 2040-08-26 01:35:00+02:00
    print('>', sd6.sunset ) # 2040-08-27 00:18:00+02:00

    print('McMurdo:')
    sd7 = Sundatetime(*pl7)
    sd7.calc_sunrise_sunset(datetime(*dt7, tzinfo=Tz(*tz7)))
    print('>', sd7.sunrise) # 2033-08-11 13:00:00+12:00
    print('>', sd7.sunset ) # 2033-08-09 13:00:00+12:00

    print('McMurdo:')
    sd8 = Sundatetime(*pl8)
    sd8.calc_sunrise_sunset(datetime(*dt8, tzinfo=Tz(*tz8)))
    print('>', sd8.sunrise) # 2033-10-21 03:06:00+13:00
    print('>', sd8.sunset ) # 2033-10-22 00:12:00+13:00

If :mod:`datetime` module is not available, the same input data can feed class
:class:`Suntime`::

    from suntime import Suntime

    st1 = Suntime(*pl1, timezone=tz1[0]*60)
    st1.calc_sunrise_sunset(*dt1, dst=tz1[1]*60)
    print('Rome:')
    print('>', divmod(st1.sunrise, 60)) # (7, 40)
    print('>', divmod(st1.sunset , 60)) # (16, 47)

    st2 = Suntime(*pl2, timezone=tz2[0]*60)
    st2.calc_sunrise_sunset(*dt2, dst=tz2[1]*60)
    print('Vancouver:')
    print('>', divmod(st2.sunrise, 60)) # (7, 16)
    print('>', divmod(st2.sunset , 60)) # (18, 46)

    st3 = Suntime(*pl3, timezone=tz3[0]*60)
    st3.calc_sunrise_sunset(*dt3, dst=tz3[1]*60)
    print('Cape Town:')
    print('>', divmod(st3.sunrise, 60)) # (5, 32)
    print('>', divmod(st3.sunset , 60)) # (19, 57)

    st4 = Suntime(*pl4, timezone=tz4[0]*60)
    st4.calc_sunrise_sunset(*dt4, dst=tz4[1]*60)
    print('Novosibirsk:')
    print('>', divmod(st4.sunrise, 60)) # (-1, 4)
    print('>', divmod(st4.sunset , 60)) # (13, 49)

    st5 = Suntime(*pl5, timezone=tz5[0]*60)
    st5.calc_sunrise_sunset(*dt5, dst=tz5[1]*60)
    print('Pyramiden:')
    print('>', divmod(st5.sunrise, 60)) # (-12, 57)
    print('>', divmod(st5.sunset , 60)) # (36, 57)

    st6 = Suntime(*pl6, timezone=tz6[0]*60)
    st6.calc_sunrise_sunset(*dt6, dst=tz6[1]*60)
    print('Pyramiden:')
    print('>', divmod(st6.sunrise, 60)) # (1, 35)
    print('>', divmod(st6.sunset , 60)) # (24, 18)

    st7 = Suntime(*pl7, timezone=tz7[0]*60)
    st7.calc_sunrise_sunset(*dt7, dst=tz7[1]*60)
    print('McMurdo:')
    print('>', divmod(st7.sunrise, 60)) # (37, 0)
    print('>', divmod(st7.sunset , 60)) # (-11, 0)

    st8 = Suntime(*pl8, timezone=tz8[0]*60)
    st8.calc_sunrise_sunset(*dt8, dst=tz8[1]*60)
    print('McMurdo:')
    print('>', divmod(st8.sunrise, 60)) # (3, 6)
    print('>', divmod(st8.sunset , 60)) # (24, 12)
