// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

/* Auto-generated config file hpl_nvmctrl_config.h */
#pragma once

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Basic Settings

// <o> Power Reduction Mode During Sleep
// <0x00=> Wake On Access
// <0x01=> Wake Up Instant
// <0x03=> Disabled
// <id> nvm_arch_sleepprm
#ifndef CONF_NVM_SLEEPPRM
#define CONF_NVM_SLEEPPRM 0
#endif

// <q> AHB0 Cache Disable
// <i> Indicate whether AHB0 cache is disable or not
// <id> nvm_arch_cache0
#ifndef CONF_NVM_CACHE0
#define CONF_NVM_CACHE0 1
#endif

// <q> AHB1 Cache Disable
// <i> Indicate whether AHB1 cache is disable or not
// <id> nvm_arch_cache1
#ifndef CONF_NVM_CACHE1
#define CONF_NVM_CACHE1 1
#endif

// </h>

// <<< end of configuration section >>>
