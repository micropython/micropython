#ifndef MICROPY_INCLUDED_STM32_NIMBLE_NIMBLE_NPL_OS_H
#define MICROPY_INCLUDED_STM32_NIMBLE_NIMBLE_NPL_OS_H

#include <stdint.h>

#define BLE_NPL_OS_ALIGNMENT (4)
#define BLE_NPL_TIME_FOREVER (0xffffffff)

typedef uint32_t ble_npl_time_t;
typedef int32_t ble_npl_stime_t;

struct ble_npl_event {
    ble_npl_event_fn *fn;
    void *arg;
    struct ble_npl_event *prev;
    struct ble_npl_event *next;
};

struct ble_npl_eventq {
    struct ble_npl_event *head;
    struct ble_npl_eventq *nextq;
};

struct ble_npl_callout {
    bool active;
    uint32_t ticks;
    struct ble_npl_eventq *evq;
    struct ble_npl_event ev;
    struct ble_npl_callout *nextc;
};

struct ble_npl_mutex {
    volatile uint8_t locked;
};

struct ble_npl_sem {
    volatile uint16_t count;
};

#endif // MICROPY_INCLUDED_STM32_NIMBLE_NIMBLE_NPL_OS_H
