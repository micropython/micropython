// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

void reset_sercoms(void);
void allow_reset_sercom(Sercom *sercom);
void never_reset_sercom(Sercom *sercom);
