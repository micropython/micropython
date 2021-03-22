// CircuitPython: Adapted from Zephyer include files.
/** @file
 *  @brief Bluetooth device address definitions and utilities.
 */

/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 * Copyright 2020 Dan Halbert for Adafruit Industries
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_BLUETOOTH_ADDR_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_ADDR_H_

#include <stdbool.h>
#include <string.h>

/**
 * @brief Bluetooth device address definitions and utilities.
 * @defgroup bt_addr Device Address
 * @ingroup bluetooth
 * @{
 */

#define BT_ADDR_LE_PUBLIC       0x00
#define BT_ADDR_LE_RANDOM       0x01
#define BT_ADDR_LE_PUBLIC_ID    0x02
#define BT_ADDR_LE_RANDOM_ID    0x03

/** Bluetooth Device Address */
typedef struct {
    uint8_t val[6];
} bt_addr_t;

/** Bluetooth LE Device Address */
typedef struct {
    uint8_t type;
    bt_addr_t a;
} bt_addr_le_t;

#define BT_ADDR_ANY     ((bt_addr_t[]) { { { 0, 0, 0, 0, 0, 0 } } })
#define BT_ADDR_NONE    ((bt_addr_t[]) { { \
                                             { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } } })
#define BT_ADDR_LE_ANY  ((bt_addr_le_t[]) { { 0, { { 0, 0, 0, 0, 0, 0 } } } })
#define BT_ADDR_LE_NONE ((bt_addr_le_t[]) { { 0, \
                                              { { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } } } })

static inline int bt_addr_cmp(const bt_addr_t *a, const bt_addr_t *b) {
    return memcmp(a, b, sizeof(*a));
}

static inline int bt_addr_le_cmp(const bt_addr_le_t *a, const bt_addr_le_t *b) {
    return memcmp(a, b, sizeof(*a));
}

static inline void bt_addr_copy(bt_addr_t *dst, const bt_addr_t *src) {
    memcpy(dst, src, sizeof(*dst));
}

static inline void bt_addr_le_copy(bt_addr_le_t *dst, const bt_addr_le_t *src) {
    memcpy(dst, src, sizeof(*dst));
}

#define BT_ADDR_IS_RPA(a)     (((a)->val[5] & 0xc0) == 0x40)
#define BT_ADDR_IS_NRPA(a)    (((a)->val[5] & 0xc0) == 0x00)
#define BT_ADDR_IS_STATIC(a)  (((a)->val[5] & 0xc0) == 0xc0)

#define BT_ADDR_SET_RPA(a)    ((a)->val[5] = (((a)->val[5] & 0x3f) | 0x40))
#define BT_ADDR_SET_NRPA(a)   ((a)->val[5] &= 0x3f)
#define BT_ADDR_SET_STATIC(a) ((a)->val[5] |= 0xc0)

int bt_addr_le_create_nrpa(bt_addr_le_t *addr);
int bt_addr_le_create_static(bt_addr_le_t *addr);

static inline bool bt_addr_le_is_rpa(const bt_addr_le_t *addr) {
    if (addr->type != BT_ADDR_LE_RANDOM) {
        return false;
    }

    return BT_ADDR_IS_RPA(&addr->a);
}

static inline bool bt_addr_le_is_identity(const bt_addr_le_t *addr) {
    if (addr->type == BT_ADDR_LE_PUBLIC) {
        return true;
    }

    return BT_ADDR_IS_STATIC(&addr->a);
}

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_ADDR_H_ */
