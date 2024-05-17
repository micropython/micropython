// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_BUSIO_INIT_H
#define MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_BUSIO_INIT_H

void reset_sercoms(void);
void allow_reset_sercom(Sercom *sercom);
void never_reset_sercom(Sercom *sercom);


#endif // MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_BUSIO_INIT_H
