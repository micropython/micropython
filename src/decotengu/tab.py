#
# DecoTengu - dive decompression library.
#
# Copyright (C) 2013-2018 by Artur Wroblewski <wrobell@riseup.net>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

"""
.. _tab-calc:

Tabular Calculations
--------------------
To calculate saturation of inert gas in tissue compartments, the
decompression model uses Schreiner equation, which calls exponential
function. Such call can be too costly or even impossible on some of the
hardware architectures, i.e.  microcontrollers lacking FPU. To make
decompression software available on such hardware architectures, the
exponential function values can be precomputed and stored in a table.
DecoTengu library allows to experiment with dive decompression calculations
using such technique, which we call tabular calculation.

The tabular calculation uses group homomorphism of exponential function

    .. math::

        e^{x + y} = e^x * e^y

Part of Schreiner equation (see :ref:`eq-schreiner`) is calculation of
inert gas exposure with exponential function

    .. math::

        e^{-k * t}

We can precompute the values for above function for 1 minute and for
6 seconds intervals and store them in a table for each gas decay constant
:math:`k`. Then, exponential function results can be calculated with
formula

    .. math::

        e^{-k * t} = (e_{1min}) ^ {n_{1min}} * (e_{6s}) ^ {n_{6s}}

where

:math:`e_{1min}`
    Exponential function value :math:`e^{-k * 1}`.

:math:`e_{6s}`
    Exponential function value :math:`e^{-k * 0.1}` (:math:`0.1` is
    6 seconds).

:math:`n_{1min}`
    Number of one minute time intervals :math:`t\ div\ 1` in time :math:`t`.

:math:`n_{6s}`
    Number of 6 second time intervals :math:`(t\ mod\ 1) * 10` in time
    :math:`t\ mod\ 1`.

The precalculated values of exponential function imply configuration
constraints, which are discussed in the following section.

.. _tab-conf:

Configuration Constraints
~~~~~~~~~~~~~~~~~~~~~~~~~
The main configuration constraint for DecoTengu tabular calculation is the
number of precomputed values of exponential function. This is driven by
limited amount of computer memory.

To keep the number of precomputed values of exponential function constant,
we need to make ascent and descent rates constant. Therefore, first
configuration constraint sets ascent and descent rates to 10m/min.

The second configuration constraint is the smallest depth change for which
tissue saturation can be calculated. By default it is 1m (or 6s at
10m/min), which forces us to round up current depth value, i.e. from 31.2m
to 32m.

.. _tab-algo:

Implementation
~~~~~~~~~~~~~~
The values of exponential function are precomputed and stored by tabular
calculator class :py:class:`decotengu.alt.tab.TabExp`.

The :py:class:`decotengu.alt.tab.TabExp` class divides time into
one minute segments and 6 seconds segments and caclulates exponential
function value using formula specified at the begining of this section.

The helper function :py:func:`decotengu.alt.tab.tab_engine` takes
decompression engine object as an argument and overrides engine
configuration and methods, so decompression calculations can be performed
with tabular calculator.

Example
~~~~~~~
To calculate dive decompression information using tabular calculator
override decompression engine object with
:py:func:`decotengu.alt.tab.tab_engine` function.

Create the decompression engine first

    >>> import decotengu
    >>> from decotengu.alt.tab import tab_engine
    >>> engine = decotengu.create()
    >>> engine.add_gas(0, 21)

Override the engine

    >>> tab_engine(engine)
    >>> print(engine.model._exp) # doctest:+ELLIPSIS
    <decotengu.alt.tab.TabExp object at ...>

Perform calculations

    >>> profile = list(engine.calculate(35, 40))
    >>> for stop in engine.deco_table:
    ...     print(stop)
    DecoStop(depth=18.0, time=1.0)
    DecoStop(depth=15.0, time=1.0)
    DecoStop(depth=12.0, time=4.0)
    DecoStop(depth=9.0, time=6.0)
    DecoStop(depth=6.0, time=9.0)
    DecoStop(depth=3.0, time=22.0)
"""

import math

from . import const


TIME_6S = 0.1
EXP = math.exp

class TabExp():
    """
    Tabular calculator.

    Calculate value of exponential function using precomputed values for
    `exp` function.

    :var _kt_exp: Collection of precomputed values of exp function for
        nitrogen and helium decay constants :math:`k`.
    """
    def __init__(self, n2_k_const, he_k_const):
        """
        Create instance of tabular calculator.

        The precomputed values of exponential function are calculated.
        """
        self._kt_exp = self._calc_exp(n2_k_const)
        self._kt_exp.update(self._calc_exp(he_k_const))


    def _calc_exp(self, k_const):
        """
        For each gas decay constant :math:`k` calculate value of
        exponential function for 1min and 6s.

        :param k_const: Collection of gas decay constants :math:`k`.
        """
        kt_exp = {}
        for k in k_const:
            exp = lambda t: EXP(-k * t)
            kt_exp[k] = {
                6: exp(TIME_6S),  # 1m at 10m/min
                60: exp(1),       # 10m at 10m/min
            }

        return kt_exp


    def __call__(self, time, k):
        """
        Calculate exponential function value using precomputed values and
        the property

            .. math::

                e^{x + y} = e^x * e^y

        :param time: Time of exposure [min].
        :param k: Gas decay constant :math:`k` for a tissue compartment.
        """
        kt_exp = self._kt_exp[k]
        n1 = round(time // 1)
        n2 = round(time % 1 * 10)
        result = kt_exp[60] ** n1 * kt_exp[6] ** n2

        return result

