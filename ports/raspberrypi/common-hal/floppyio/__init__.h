// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// empirical-ish from RP2040 @ 125MHz for floppy_flux_readinto
#define FLOPPYIO_SAMPLERATE (24000000)
// empirical-ish from RP2040 @ 125MHz for floppy_mfm_readinto
// my guess is these are slower because the more complex routine falls out of cache, but it's just
// speculation because the loops are very similar. When looking at raw bins with a modified
// version of adafruit_floppy, it can be seen that there are _two_ peaks for T2 and T3, rather
// than a single one, around 36 (mostly) and 43 (rarer), compared to a single peak around 48.
#define T2_5 (54)
#define T3_5 (75)
