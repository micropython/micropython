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
DecoTengu dive decompression engine.

[mpdfd] Powell, Mark. Deco for Divers, United Kingdom, 2010
"""

import gc
gc.collect

from ucollections import namedtuple

import math
import utime
import gc

from .model import ZH_L16B_GF, Data
from .error import ConfigError, EngineError
from .ft import recurse_while, bisect_find
from . import const

def timed_function(f, *args, **kwargs):
    myname = str(f)
    def new_func(*args, **kwargs):
        t = utime.ticks_us()
        result = f(*args, **kwargs)
        delta = utime.ticks_diff(utime.ticks_us(), t)
        print('Function {} Time = {:6.3f}ms'.format(myname, delta/1000))
        return result
    return new_func


class Phase(object):
    """
    Dive phase enumeration.

    The dive phases are

    START
        Start of a dive. It happens at begining of the dive (time=0min,
        depth=0min). Only one dive step can exist with such dive phase.
    DESCENT
        Descent during dive - current dive step is deeper than previous one.
    CONST
        Constant depth during dive - current dive step is at the same depth as
        previous one.
    ASCENT
        Ascent during dive - current dive step is shallower than previous one.
    DECO_STOP
        Decompression stop. Current dive step is at the same depth as previous
        one and ascent is not possible until allowed by decompression model.
    GAS_SWITCH
        Gas mix switch. Current dive step is at the same depth as previous
        one. The time of current and previous dive steps is the same.
    """
    START = 'start'
    DESCENT = 'desc'
    CONST = 'const'
    ASCENT = 'asc'
    DECO_STOP = 'deco'
    GAS_SWITCH = 'gas'


Step = namedtuple('Step', 'phase abs_p time gas data')

GasMix = namedtuple('GasMix', 'depth o2 n2 he')

DecoStop = namedtuple('DecoStop', 'depth time')

class Engine(object):
    """
    DecoTengu decompression engine.

    Use decompression engine to calculate dive profile and decompression
    information.

    :var model: Decompression model.
    :var surface_pressure: Surface pressure [bar].
    :var ascent_rate: Ascent rate during a dive [m/min].
    :var descent_rate: Descent rate during a dive [m/min].
    :var last_stop_6m: If true, then last deco stop is at 6m (not default 3m).
    :var deco_table: List of decompression stops.
    :var _gas_list: List of gas mixes.
    :var _deco_stop_search_time: Time limit for decompression stop linear
        search.
    """
    def __init__(self, model=ZH_L16B_GF()):
        super().__init__()
        self.model = model
        self.surface_pressure = const.SURFACE_PRESSURE
        self.ascent_rate = 10.0
        self.descent_rate = 20.0
        self.last_stop_6m = False
        self.deco_table = DecoTable()

        self._gas_list = []
        self._travel_gas_list = []

        self._deco_stop_search_time = const.DECO_STOP_SEARCH_TIME

        self._meter_to_bar = const.METER_TO_BAR
        self._p3m = 3 * const.METER_TO_BAR


    def _to_pressure(self, depth):
        """
        Convert depth in meters to absolute pressure in bars.

        :param depth: Depth in meters.
        """
        return depth * self._meter_to_bar + self.surface_pressure

    def _to_depth(self, abs_p):
        """
        Convert absolute pressure to depth.

        :param abs_p: Absolute pressure of depth [bar].
        """
        depth = (abs_p - self.surface_pressure) / self._meter_to_bar
        return round(depth, const.SCALE)

    def _time_to_pressure(self, time, rate):
        """
        Convert time into pressure change using depth change rate.

        :param time: Time [min].
        :param rate: Rate of depth change [m/min].
        """
        return time * rate * self._meter_to_bar


    def _pressure_to_time(self, pressure, rate):
        """
        Convert pressure change into time using depth change rate.

        The returned time is in minutes.

        :param pressure: Pressure change [bar].
        :param rate: Rate of depth change [m/min].
        """
        return pressure / rate / self._meter_to_bar


    def _ceil_pressure_3m(self, abs_p):
        """
        Calculate absolute pressure value, so when converted to meters its
        value is divisible by 3.

        :param abs_p: Input absolute pressure [bar].
        """
        v = math.ceil((abs_p - self.surface_pressure) / self._p3m)
        return  v * self._p3m + self.surface_pressure


    def _n_stops(self, start_abs_p, end_abs_p=0.0):
        """
        Calculate amount of decompression stops required between start and
        end depths.

        :param start_abs_p: Absolute pressure of starting depth.
        :param end_abs_p: Absolute pressure of ending depth (surface
            pressure if null).
        """
        if end_abs_p is 0.0:
            end_abs_p = self.surface_pressure
        k = (start_abs_p - end_abs_p) // self._p3m 
        return k


    def _inv_limit(self, abs_p, data):
        """
        Return true if decompression model data does not violate
        decompression model ceiling limit invariant.

        The invariant is

            Absolute pressure (depth) has to be deeper or at the same depth
            as absolute pressure of ceiling limit.

        :param abs_p: Absolute pressure of current depth.
        :param data: Decompression model data.
        """
        return abs_p >= self.model.ceiling_limit(data)


    def _can_ascend(self, abs_p, time, data, gf=None):
        """
        Check if a diver can ascend from current depth without violating
        ascent ceiling limit.

        :param abs_p: Absolute pressure of current depth [bar].
        :param time: Time of ascent [min].
        :param data: Decompression model data.
        :param gf: Gradient factor to be used for ceiling check.
        """
        p = abs_p - self._time_to_pressure(time, self.ascent_rate)
        return p >= self.model.ceiling_limit(data, gf=gf)


    def _step_start(self, abs_p, gas):
        """
        Create the very first dive step.

        The first step is initialized with decompression data calculated
        for surface.

        The dive starting depth is usually surface, but any depth can be
        specified, i.e. when descent part of the dive is to be skipped.

        :param abs_p: Absolute pressure of dive starting depth.
        :param gas: Gas mix configuration.
        """
        data = self.model.init(self.surface_pressure)
        step = Step(Phase.START, abs_p, 0, gas, data)
        return step


    def _step_next(self, step, time, gas, phase='const'):
        """
        Calculate next dive step at constant depth and advanced by
        specified amount of time.

        :param step: Current dive step.
        :param time: Time spent at current depth [min].
        :param gas: Gas mix configuration.
        :param data: Decompression model data.
        :param phase: Dive phase.
        """
        data = self._tissue_pressure_const(step.abs_p, time, gas, step.data)
        return Step(phase, step.abs_p, step.time + time, gas, data)


    def _step_next_descent(self, step, time, gas, phase='descent'):
        """
        Calculate next dive step when descent is performed for specified
        period of time.

        :param step: Current dive step.
        :param time: Time to descent from current dive step [min].
        :param gas: Gas mix configuration.
        :param phase: Dive phase.
        """
        data = self._tissue_pressure_descent(step.abs_p, time, gas, step.data)
        pressure = step.abs_p + self._time_to_pressure(time, self.descent_rate)
        return Step(phase, pressure, step.time + time, gas, data)


    @micropython.native
    def _step_next_ascent(self, step, time, gas, gf=None, phase='ascent'):
        """
        Calculate next dive step when ascent is performed for specified
        period of time.

        FIXME: due to ``gf`` parameter this method is deco model dependant,
               this has to be improved

        :param step: Current dive step.
        :param time: Time to ascent from current dive step [min].
        :param gas: Gas mix configuration.
        :param data: Decompression model data.
        :param phase: Dive phase.
        """
        data = self._tissue_pressure_ascent(step.abs_p, time, gas, step.data)
        pressure = step.abs_p - self._time_to_pressure(time, self.ascent_rate)
        if gf is not None:
            # FIXME: make it model independent
            data = Data(data.tissues, gf)
        return Step(phase, pressure, step.time + time, gas, data)


    def _tissue_pressure_const(self, abs_p, time, gas, data):
        """
        Calculate tissues gas loading after exposure for specified amount
        of time at depth.

        :param abs_p: Absolute pressure indicating the depth [bar].
        :param time: Time at depth [min].
        :param gas: Gas mix configuration.
        :param data: Decompression model data.
        """
        return self.model.load(abs_p, time, gas, 0, data)


    def _tissue_pressure_descent(self, abs_p, time, gas, data):
        """
        Calculate tissues gas loading after descent.

        :param abs_p: Starting pressure indicating the depth [bar].
        :param time: Time of descent [min].
        :param gas: Gas mix configuration.
        :param data: Decompression model data.
        """
        rate = self.descent_rate * self._meter_to_bar
        data = self.model.load(abs_p, time, gas, rate, data)
        return data


    def _tissue_pressure_ascent(self, abs_p, time, gas, data):
        """
        Calculate tissues gas loading after ascent.

        :param abs_p: Starting pressure indicating the depth [bar].
        :param time: Time of ascent [min].
        :param gas: Gas mix configuration.
        :param data: Decompression model data.
        """
        rate = -self.ascent_rate * self._meter_to_bar
        tp = self.model.load(abs_p, time, gas, rate, data)
        return tp


    def _switch_gas(self, step, gas):
        """
        Switch gas mix.

        The switch results in new dive step.
        """
        step = Step(Phase.GAS_SWITCH, step.abs_p, step.time, gas, step.data)
        return step


    def _dive_descent(self, abs_p, gas_list):
        """
        Dive descent from surface to absolute pressure of destination
        depth.

        The last gas on the gas mix list is bottom gas, others are travel
        gas mixes.

        :param abs_p: Absolute pressure of destination depth.
        :param gas_list: List of gas mixes - travel and bottom gas mixes.
        """
        gas = gas_list[0]
        step = self._step_start(self.surface_pressure, gas)
        gc.collect()
        yield step

        stages = self._descent_stages(abs_p, gas_list)
        for i, (depth, gas) in enumerate(stages):
            if i > 0: # perform gas switch
                step = self._switch_gas(step, gas)
                gc.collect()
                yield step
            time = self._pressure_to_time(depth - step.abs_p, self.descent_rate)
            step = self._step_next_descent(step, time, gas)
            gc.collect()
            yield step

        last = gas_list[-1]
        if abs(step.abs_p - self._to_pressure(last.depth)) < const.EPSILON:
            assert gas != last
            step = self._switch_gas(step, last)
            gc.collect()
            yield step


    def _dive_ascent(self, start, gas_list):
        """
        Dive ascent from starting dive step.

        The method checks if the ascent is part of NDL dive before dive
        ascent starts.

        If dive is decompression dive, then ascent is divided into two
        phases

        - ascent to first decompression stop
        - ascent performing decompression stops

        :param start: Starting dive step.
        :param gas_list: List of gas mixes - bottom and decompression gas
            mixes.
        """
        # check if ndl dive
        bottom_gas = gas_list[0]
        step = self._ndl_ascent(start, bottom_gas)
        if step:
            gc.collect()
            yield step
            return

        step = start

        stages = self._free_ascent_stages(gas_list)
        for step in self._free_staged_ascent(step, stages):
            gc.collect()
            yield step

        # we should not arrive at the surface - it is non-ndl dive at this
        # stage
        assert not abs(step.abs_p - self.surface_pressure) < const.EPSILON

        stages = self._deco_ascent_stages(step.abs_p, gas_list)
        gc.collect()
        yield from self._deco_staged_ascent(step, stages)


    def _ndl_ascent(self, start, gas):
        """
        Check if NDL ascent to the surface is possible from starting dive
        step.

        Return the surface dive step if NDL ascent is possible, null
        otherwise.

        NDL ascent is performed to the surface usually using bottom gas
        (NOTE: not always possible - exceptions not implemented yet).

        To calculate surface dive step, the surface decompression model
        parameters are applied, i.e. for ZH-L16-GF decompression model,
        gradient factor value is set to GF high parameter.

        :param start: Starting dive step.
        :param gas: Gas mix used during NDL ascent.
        """
        # FIXME: method is decompression model dependant

        gf = self.model.gf_high
        p = start.abs_p - self.surface_pressure
        time = self._pressure_to_time(p, self.ascent_rate)
        step = self._step_next_ascent(start, time, gas, gf=gf)
        limit = self.model.ceiling_limit(step.data, gf)
        if step.abs_p < limit:
            step = None
        return step


    def _find_first_stop(self, start, abs_p, gas):
        """
        Find first first decompression stop using Schreiner equation.

        Method returns dive step - start of first decompression stop.

        Below, by depth we mean absolute pressure of depth expressed in
        bars.

        The depth of first decompression stop is the shallowest depth,
        which does not breach the ascent limit imposed by ascent ceiling.
        The depth is divisble by 3.

        The first decompression stop depth is found by ascending to
        adjusted value of current ascent ceiling limit. The current ascent
        ceiling value is always adjusted, so its value in meters is
        divisible by 3. The ascent is repeated while it is possible to do
        so and until target depth.

        :param start: Starting dive step indicating current depth.
        :param abs_p: Absolute pressure of target depth - surface or gas
            switch depth.
        :param gas: Gas mix configuration.
        """
        assert start.abs_p > abs_p, '{} vs. {}'.format(start.abs_p, abs_p)
        assert self._to_depth(abs_p) % 3 == 0, self._to_depth(abs_p)

        model = self.model

        step = start
        limit = model.ceiling_limit(step.data, step.data.gf)
        limit = self._ceil_pressure_3m(limit)
        limit = max(abs_p, limit)
        t = self._pressure_to_time(step.abs_p - limit, self.ascent_rate)

        while step.abs_p > limit and step.abs_p > abs_p:
            step = self._step_next_ascent(step, t, gas)
            limit = model.ceiling_limit(step.data, step.data.gf)
            limit = self._ceil_pressure_3m(limit)
            limit = max(abs_p, limit)
            t = self._pressure_to_time(step.abs_p - limit, self.ascent_rate)

        stop = step

        assert stop.abs_p - abs_p > -const.EPSILON, stop

        return stop


    def _descent_stages(self, end_abs_p, gas_list):
        """
        Calculate stages for dive descent.

        Descent stage is a tuple

        - absolute pressure of destination depth
        - gas mix

        The descent stages are calculated using gas mix list. The absolute
        pressure of destination depth is switch depth of next gas mix
        absolute pressure of destination depth, for
        example for `end_abs_p = 6.6bar`::

             0m  30%        4.6bar (36m)  30%
            36m  21%   ->   6.6bar (56m)  21%

        If switch depth of last gas mix is equal to the destination depth,
        then descent stage is not included for it. It means that descent
        is performed to the bottom on travel gas only and it is
        responsbility of the caller to perform appropriate bottom gas
        switch.

        :param end_abs_p: Absolute pressure of destination depth.
        :param gas_list: List of gas mixes - travel and bottom gas mixes.
        """
        mixes = zip(gas_list[:-1], gas_list[1:])
        _pressure = lambda mix: self._to_pressure(mix.depth)
        yield from ((_pressure(m2), m1) for m1, m2 in mixes)
        last = gas_list[-1]
        if abs(_pressure(last) - end_abs_p) > 0:
            gc.collect()
            yield (end_abs_p, last)


    def _free_ascent_stages(self, gas_list):
        """
        Calculate stages for deco-free ascent.

        Ascent stage is a tuple

        - absolute pressure of destination depth
        - gas mix

        The ascent stages are calculated using gas mix list. The absolute
        pressure of destination depth is gas switch depth rounded up to
        multiply of 3m and then converted to pressure, for example::

             0m  21%        3.4bar (24m)  21%
            22m  50%   ->   1.6bar  (6m)  50%
             6m 100%        1.0bar  (0m) 100%

        :param gas_list: List of gas mixes - bottom and decompression gas
            mixes.
        """
        mixes = zip(gas_list[:-1], gas_list[1:])
        _pressure = lambda mix: \
            self._to_pressure(((mix.depth - 1) // 3 + 1) * 3)
        yield from ((_pressure(m2), m1) for m1, m2 in mixes)
        gc.collect()
        yield (self.surface_pressure, gas_list[-1])


    def _deco_ascent_stages(self, start_abs_p, gas_list):
        """
        Calculate stages for decompression ascent.

        Ascent stage is a tuple

        - absolute pressure of destination depth
        - gas mix

        The ascent stages are calculated using gas mix list. The absolute
        pressure of destination depth is gas switch depth rounded down to
        multiply of 3m and then converted to pressure, for example::

             0m  21%         3.1bar (21m)  21%
            22m  50%   ->    1.6bar (6m)   50%
             6m 100%         1.0bar (0m)  100%

        Only gas mixes, which switch depth is shallower than start depth,
        are used for decompression ascent stages calculation.

        :param gas_list: List of gas mixes - bottom and decompression gas
            mixes.
        :param start_abs_p: Absolute pressure of decompression start depth.
        """
        assert start_abs_p > self.surface_pressure
        mixes = zip(gas_list[:-1], gas_list[1:])
        _pressure = lambda mix: self._to_pressure(mix.depth // 3 * 3)
        yield from (
            (_pressure(m2), m1) for m1, m2 in mixes
            if self._to_pressure(m2.depth) < start_abs_p
        )
        gc.collect()
        yield (self.surface_pressure, gas_list[-1])


    def _validate_gas_list(self, depth):
        """
        Validate gas mix list.

        `ConfigError` is raised if any of gas mix rules are violated.

        The gas mix rules are

        #. There is one non-travel gas mix on gas mix list.
        #. If no travel gas mixes, then first gas mix is bottom gas and its
           switch depth is 0m.
        #. All travel gas mixes have different switch depth.
        #. All decompression gas mixes have different switch depth.
        #. All decompression gas mixes have switch depth greater than zero.
        #. There is no gas mix with switch depth deeper than maximum dive
           depth.

        :param depth: Maximum dive depth.
        """
        if not self._gas_list:
            raise ConfigError('No bottom gas mix configured')

        if not self._travel_gas_list and self._gas_list[0].depth != 0:
            raise ConfigError('Bottom gas mix switch depth is not 0m')

        k = len(self._travel_gas_list)
        depths = (m.depth for m in self._travel_gas_list)
        if k and len(set(depths)) != k:
            raise ConfigError(
                'Two or more travel gas mixes have the same switch depth'
            )

        k = len(self._gas_list[1:])
        depths = [m.depth for m in self._gas_list[1:]]
        if len(set(depths)) != k:
            raise ConfigError(
                'Two or more decompression gas mixes have the same'
                ' switch depth'
            )

        if any(d == 0 for d in depths):
            raise ConfigError('Decompression gas mix switch depth is 0m')

        mixes = self._gas_list + self._travel_gas_list
        mixes = [m for m in mixes if m.depth > depth]
        if mixes:
            raise ConfigError(
                'Gas mix switch depth deeper than maximum dive depth'
            )


    def _ascent_switch_gas(self, step, gas):
        """
        Switch to specified gas mix, ascending if necessary.

        The method is used to switch gas during dive ascent when ascent is
        performed to depth being multiply of 3m. Two scenarios are
        implemented

        #. Gas mix switch depth is the same as current dive step depth,
           then simply perform gas mix switch.
        #. Gas mix switch depth is shallower than current dive step depth

           - ascend to gas mix switch depth
           - perform gas mix switch
           - ascend to next depth, which is multiply of 3m

        Gas mix switch is done in place, takes no time at the moment, but
        in the future this should be configurable.

        A tuple of gas mix switch dive steps is returned.

        :param step: Current dive step.
        :param gas: Gas to switch to.
        """
        gp = self._to_pressure(gas.depth)
        assert step.abs_p - gp < self._p3m
        if abs(step.abs_p - gp) < const.EPSILON:
            steps = (self._switch_gas(step, gas),)
        else:
            assert step.abs_p > gp

            time = self._pressure_to_time(step.abs_p - gp, self.ascent_rate)
            s1 = self._step_next_ascent(step, time, step.gas)

            s2 = self._switch_gas(s1, gas)

            p = self._to_pressure(gas.depth // 3 * 3)
            time = self._pressure_to_time(s2.abs_p - p, self.ascent_rate)
            s3 = self._step_next_ascent(s2, time, gas)

            steps = (s1, s2, s3)
        return steps


    def _free_staged_ascent(self, start, stages):
        """
        Perform staged ascent until first decompression stop.

        :param start: Starting dive step.
        :param stages: Dive stages.

        .. seealso:: :func:`decotengu.Engine._free_ascent_stages`
        """
        step = start
        for depth, gas in stages:
            if step.gas != gas: # first step might not need gas switch
                # if gas switch drives us into deco zone, then stop ascent
                # leaving `step` as first decompression stop
                gs_steps = self._ascent_switch_gas(step, gas)
                if self._inv_limit(gs_steps[-1].abs_p, gs_steps[-1].data):
                    step = gs_steps[-1]
                    gc.collect()
                    yield from gs_steps
                else:
                    break

            # check if there is first decompression stop at this ascent
            # stage
            s = self._find_first_stop(step, depth, gas)
            if s is step:
                break # already at deco zone
            else:
                step = s
                gc.collect()
                yield step
                if abs(step.abs_p - depth) > const.EPSILON: # deco stop found
                    break
                # else: at target depth of ascent stage without deco stop,
                #       so move to next stage


    def _deco_staged_ascent(self, start, stages):
        """
        Perform staged asccent within decompression zone.

        :param start: Starting dive step.
        :param stages: Dive stages.

        .. seealso:: :func:`decotengu.Engine._ascent_stages_deco`
        """
        bottom_gas = self._gas_list[0]
        stages = self._deco_stops(start, stages)
        step = start
        for depth, gas, time, gf in stages:
            # switch gas
            if step.abs_p >= self._to_pressure(gas.depth) and gas != bottom_gas:
                for step in self._ascent_switch_gas(step, gas):
                    gc.collect()
                    yield step

            # execute deco stop
            end = self._deco_stop(step, time, gas, gf)
            self.deco_table.append(
                self._to_depth(step.abs_p),
                end.time - step.time
            )
            step = end
            gc.collect()
            yield step

            # ascend to next deco stop
            step = self._step_next_ascent(step, time, gas, gf=gf)
            gc.collect()
            yield step


    def _deco_stops(self, step, stages):
        """
        Calculate collection of decompression stops.

        The method returns collection of tuples

        - destination depth (see :func:`decotengu.Engine._deco_ascent_stages`
          method)
        - gas mix (see :func:`decotengu.Engine._deco_ascent_stages` method)
        - time required to ascent to next decompression stops (usually time
          required to ascent by 3m)
        - gradient factor value for next decompression stop or surface

        :param step: Current dive step.
        :param stages: Decompression ascent stages.

        .. seealso:: :func:`decotengu.Engine._deco_ascent_stages`
        """
        k = self._n_stops(step.abs_p)
        gf_step = (self.model.gf_high - self.model.gf_low) / k
        ts_3m = self._pressure_to_time(self._p3m, self.ascent_rate)
        gf = step.data.gf

        abs_p = step.abs_p
        stop_at_6m = self.surface_pressure + 2 * self._p3m
        ls_6m = self.last_stop_6m
        for depth, gas in stages:
            n = self._n_stops(abs_p, depth)
            for k in range(n):
                gf += gf_step
                if ls_6m and abs(abs_p - k * self._p3m - stop_at_6m) < const.EPSILON:
                    gc.collect()
                    yield depth, gas, 2 * ts_3m, gf + gf_step
                    assert abs(self.model.gf_high - gf - gf_step) < const.EPSILON
                    break
                else:
                    gc.collect()
                    yield depth, gas, ts_3m, gf
            abs_p = depth


    def _deco_stop(self, step, next_time, gas, gf):
        """
        Calculate decompression stop.

        The length of a decompression stop is guarded by gradient factor of
        next decompression stop - the current decompression stop lasts
        until it is allowed to ascent to next stop.

        :param step: Start of current decompression stop.
        :param next_time: Time required to ascent to next deco stop [min].
        :param gas: Gas mix configuration.
        :param gf: Gradient factor value of next decompression stop.
        """
        # there are a lot of 1 minute deco stops, so check if we can ascend
        # after 1 minute first; otherwise continue searching for the
        # decompression stop length
        data = self._tissue_pressure_const(
            step.abs_p, const.MINUTE, gas, step.data
        )
        if self._can_ascend(step.abs_p, next_time, data, gf):
            return Step(
                Phase.DECO_STOP, step.abs_p, step.time + const.MINUTE, gas, data
            )

        max_time = self._deco_stop_search_time
        # next_f(arg=(time, data)): (time, data) <- track both time and deco
        # data
        next_f = lambda time, data: (
            time + max_time,
            self._tissue_pressure_const(step.abs_p, max_time, gas, data)
        )
        inv_f = lambda time, data: \
            not self._can_ascend(step.abs_p, next_time, data, gf)

        time, data = recurse_while(inv_f, next_f, const.MINUTE, data)

        # start with `data` returned by `recurse_while`, so no need to add
        # `time`
        next_f = lambda k: self._tissue_pressure_const(step.abs_p, k, gas, data)
        # should we stay at deco stop?
        exec_deco_stop = lambda k: \
            not self._can_ascend(step.abs_p, next_time, next_f(k), gf)

        # ascent is possible after self._deco_stop_search_time, so
        # check for self._deco_stop_search_time - 1
        n = self._deco_stop_search_time - 1
        k = bisect_find(n, exec_deco_stop)
        k += 1 # at k diver should still stay at deco stop as
               # exec_deco_stop is true - ascent minute later

        # final time of a deco stop
        time = time + k

        step = self._step_next(step, time, gas, phase=Phase.DECO_STOP)
        return step


    def add_gas(self, depth, o2, he=0, travel=False):
        """
        Add gas mix to the gas mix list.

        First non-travel gas mix is bottom gas mix. Any other non-travel
        gas mix is decompression gas mix.

        See :func:`decotengu.engine.Engine._validate_gas_list` method
        documentation for more gas mix list rules.

        :param depth: Switch depth of gas mix.
        :param o2: O2 percentage, i.e. 80.
        :param he: Helium percentage, i.e. 18.
        :param travel: Travel gas mix if true.

        .. seealso:: :func:`decotengu.Engine._validate_gas_list`
        """
        if travel:
            self._travel_gas_list.append(GasMix(depth, o2, 100 - o2 - he, he))
        else:
            self._gas_list.append(GasMix(depth, o2, 100 - o2 - he, he))

    def calculate(self, depth, time, descent=True):
        """
        Start dive profile calculation for specified dive depth and bottom
        time.

        The method returns an iterator of dive steps.

        Before the calculation the gas mix list is validated. See
        :func:`decotengu.engine.Engine._validate_gas_list` method
        documentation for the list of gas mix list rules.

        :param depth: Maximum depth [m].
        :param time: Dive bottom time [min].
        :param descent: Skip descent part of a dive if set to false.

        .. seealso:: :func:`decotengu.Engine._validate_gas_list`
        .. seealso:: :func:`decotengu.Engine.add_gas`
        """
        del self.deco_table[:]
        self._validate_gas_list(depth)

        # prepare travel and bottom gas mixes
        bottom_gas = self._gas_list[0]
        gas_list = sorted(self._travel_gas_list, key=0)
        gas_list.append(bottom_gas)

        abs_p = self._to_pressure(depth)
        if descent:
            for step in self._dive_descent(abs_p, gas_list):
                gc.collect()
                yield step
        else:
            step = self._step_start(abs_p, bottom_gas)
            gc.collect()
            yield step

        # prepare decompression gases, first gas mix is assumed to be
        # bottom gas mix
        gas_list = sorted(self._gas_list[1:], key=0, reverse=True)
        gas_list.insert(0, bottom_gas)

        t = time - step.time
        if t <= 0:
            raise EngineError('Bottom time shorter than descent time')

        assert t > 0
        step = self._step_next(step, t, bottom_gas)
        gc.collect()
        yield step

        yield from self._dive_ascent(step, gas_list)



class DecoTable(list):
    """
    Decompression table summary.

    The class is a list of decompression stops.

    The decompression stops time is in minutes.

    .. seealso:: :class:`decotengu.engine.DecoStop`
    """
    @property
    def total(self):
        """
        Total decompression time.
        """
        return sum(s.time for s in self)


    def append(self, depth, time):
        """
        Add decompression stop.

        :param depth: Depth of decompression stop [m].
        :param time: Time of decompression stop [min].
        """

        time = round(time, const.SCALE)
        stop = DecoStop(depth, time)

        assert stop.time > 0
        assert stop.depth > 0

        super().append(stop)
