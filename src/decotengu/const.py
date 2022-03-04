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

WATER_VAPOUR_PRESSURE_Q08 = 0.0493
WATER_VAPOUR_PRESSURE_Q09 = 0.0567
# source: [mpdfd]
# used in: OSTC
WATER_VAPOUR_PRESSURE_Q10 = 0.0627
WATER_VAPOUR_PRESSURE_DEFAULT = WATER_VAPOUR_PRESSURE_Q10

SURFACE_PRESSURE = 1.01325

METER_TO_BAR = 0.09985
LOG_2 = 0.6931471805599453
SCALE = 10
EPSILON = 10 ** -SCALE
ROUND_VALUE = 0.499999
MINUTE = 1
TIME_3M = 0.3 # time [min] to ascent/descent at 10m/min

#
# number based on performance test for dive profile presented in
# Baker "Deep Stops" paper, see also
#
#   decotengu.tests.integration.test_engine.ProfileTestCase.test_deepstop
#
DECO_STOP_SEARCH_TIME = 8

# vim: sw=4:et:ai
