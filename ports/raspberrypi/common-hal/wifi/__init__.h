// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/mpconfig.h"
#include "lwip/ip_addr.h"

void wifi_reset(void);
NORETURN void raise_cyw_error(int err);
#define CHECK_CYW_RESULT(x) do { int res = (x); if (res != 0) raise_cyw_error(res); } while (0)

void ipaddress_ipaddress_to_lwip(mp_obj_t ip_address, ip_addr_t *lwip_ip_address);
