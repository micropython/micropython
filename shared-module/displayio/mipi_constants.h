// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_MIPI_CONSTANTS_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_MIPI_CONSTANTS_H

// More info here: https://www.tonylabs.com/wp-content/uploads/MIPI_DCS_specification_v1.02.00.pdf
enum mipi_command {
    MIPI_COMMAND_SET_COLUMN_ADDRESS = 0x2a,
    MIPI_COMMAND_SET_PAGE_ADDRESS = 0x2b,
    MIPI_COMMAND_WRITE_MEMORY_START = 0x2c,
};

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_MIPI_CONSTANTS_H
