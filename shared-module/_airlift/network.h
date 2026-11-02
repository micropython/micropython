// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// The ESP32's WiFi connection, as the core sees it. The status and address are
// cached, so the status bar can read them without a SPI round trip.

#include <stdbool.h>
#include <stdint.h>

// nina-fw connection status values, as in ESP32SPI.
#define AIRLIFT_WL_IDLE_STATUS (0)
#define AIRLIFT_WL_NO_SSID_AVAIL (1)
#define AIRLIFT_WL_CONNECTED (3)
#define AIRLIFT_WL_CONNECT_FAILED (4)
#define AIRLIFT_WL_DISCONNECTED (6)
#define AIRLIFT_WL_STOPPED (254)
#define AIRLIFT_WL_NO_SHIELD (255)

// Sets the network the core keeps the ESP32 on. Called at each VM start. Joins
// it unless already connected, waiting up to 10 seconds the first time since
// the last hard reset; after that, airlift_network_maintain() keeps trying in the
// background. Returns true when connected. Never raises.
bool airlift_network_join(const char *ssid, const char *password);
// Called from the background. Rejoins without waiting after a hard reset, and
// every 30 seconds while disconnected, unless VM code has joined or left a
// network itself since the core last tried.
void airlift_network_maintain(void);
// Re-reads the status and address, at most once a second unless forced. Call it
// only where an SPI command may run.
void airlift_network_refresh(bool force);
// The cached status. AIRLIFT_WL_NO_SHIELD when the core doesn't own the ESP32.
uint8_t airlift_network_status(void);
// The cached IPv4 address in network byte order, or 0 when not connected.
uint32_t airlift_network_ipv4_address(void);
