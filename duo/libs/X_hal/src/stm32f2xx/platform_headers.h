/*
 * File:   platform_headers.h
 * Author: mat
 *
 * Created on 31 October 2014, 21:25
 */

#ifndef PLATFORM_HEADERS_H
#define	PLATFORM_HEADERS_H

#ifdef	__cplusplus
extern "C" {
#endif

// These headers contain platform-specific defines.
#include "gpio_hal.h"
#include "hw_config.h"
#include "pinmap_impl.h"
#include "deepsleep_hal_impl.h"
#if PLATFORM_ID == 10 // Electron
#include "modem/enums_hal.h"
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* PLATFORM_HEADERS_H */

