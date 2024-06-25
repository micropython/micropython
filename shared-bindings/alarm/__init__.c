// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"

#if CIRCUITPY_ESPULP
#include "bindings/espulp/ULPAlarm.h"
#endif

#include "shared-bindings/alarm/__init__.h"
#include "shared-bindings/alarm/SleepMemory.h"
#include "shared-bindings/alarm/pin/PinAlarm.h"
#include "shared-bindings/alarm/time/TimeAlarm.h"
#include "shared-bindings/alarm/touch/TouchAlarm.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/supervisor/Runtime.h"
#include "shared-bindings/time/__init__.h"
#include "supervisor/shared/workflow.h"

//| """Alarms and sleep
//|
//| Provides alarms that trigger based on time intervals or on external events, such as pin
//| changes.
//| The program can simply wait for these alarms, or go to sleep and be awoken when they trigger.
//|
//| There are two supported levels of sleep: light sleep and deep sleep.
//|
//| Light sleep keeps sufficient state so the program can resume after sleeping.
//| It does not shut down WiFi, BLE, or other communications, or ongoing activities such
//| as audio playback. It reduces power consumption to the extent possible that leaves
//| these continuing activities running. In some cases there may be no decrease in power consumption.
//|
//| Deep sleep shuts down power to nearly all of the microcontroller including the CPU and RAM. This can save
//| a more significant amount of power, but CircuitPython must restart ``code.py`` from the beginning when
//| awakened.
//|
//| For both light sleep and deep sleep, if CircuitPython is connected to a host computer,
//| maintaining the connection takes priority and power consumption may not be reduced.
//|
//| For more information about working with alarms and light/deep sleep in CircuitPython,
//| see `this Learn guide <https://learn.adafruit.com/deep-sleep-with-circuitpython>`_.
//| """

//| sleep_memory: SleepMemory
//| """Memory that persists during deep sleep.
//| This object is the sole instance of `alarm.SleepMemory`."""

//| wake_alarm: Optional[circuitpython_typing.Alarm]
//| """The most recently triggered alarm. If CircuitPython was sleeping, the alarm that woke it from sleep.
//| If no alarm occurred since the last hard reset or soft restart, value is ``None``.
//| """
//|

// wake_alarm is implemented as a dictionary entry, so there's no code here.

static void validate_objs_are_alarms(size_t n_args, const mp_obj_t *objs) {
    for (size_t i = 0; i < n_args; i++) {
        if (mp_obj_is_type(objs[i], &alarm_pin_pinalarm_type) ||
            #if CIRCUITPY_ALARM_TOUCH
            mp_obj_is_type(objs[i], &alarm_touch_touchalarm_type) ||
            #endif
            #if CIRCUITPY_ESPULP
            mp_obj_is_type(objs[i], &espulp_ulpalarm_type) ||
            #endif
            mp_obj_is_type(objs[i], &alarm_time_timealarm_type)) {
            continue;
        }
        mp_raise_TypeError_varg(MP_ERROR_TEXT("Expected a kind of %q"), MP_QSTR_Alarm);
    }
}

//| def light_sleep_until_alarms(
//|     *alarms: circuitpython_typing.Alarm,
//| ) -> circuitpython_typing.Alarm:
//|     """Go into a light sleep until awakened one of the alarms. The alarm causing the wake-up
//|     is returned, and is also available as `alarm.wake_alarm`.
//|
//|     If no alarms are specified, return immediately.
//|
//|     **If CircuitPython is connected to a host computer, the connection will be maintained,
//|     and the microcontroller may not actually go into a light sleep.**
//|     This allows the user to interrupt an existing program with ctrl-C,
//|     and to edit the files in CIRCUITPY, which would not be possible in true light sleep.
//|     Thus, to use light sleep and save significant power,
//|     it may be necessary to disconnect from the host.
//|     """
//|     ...
//|
static mp_obj_t alarm_light_sleep_until_alarms(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        return mp_const_none;
    }

    validate_objs_are_alarms(n_args, args);

    mp_obj_t alarm = common_hal_alarm_light_sleep_until_alarms(n_args, args);
    shared_alarm_save_wake_alarm(alarm);
    return alarm;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(alarm_light_sleep_until_alarms_obj, 1, MP_OBJ_FUN_ARGS_MAX, alarm_light_sleep_until_alarms);

//| def exit_and_deep_sleep_until_alarms(
//|     *alarms: circuitpython_typing.Alarm, preserve_dios: Sequence[digitalio.DigitalInOut] = ()
//| ) -> None:
//|     """Exit the program and go into a deep sleep, until awakened by one of the alarms.
//|     This function does not return.
//|
//|     When awakened, the microcontroller will restart and will run ``boot.py`` and ``code.py``
//|     from the beginning.
//|
//|     After restart, an alarm *equivalent* to the one that caused the wake-up
//|     will be available as `alarm.wake_alarm`.
//|     Its type and/or attributes may not correspond exactly to the original alarm.
//|     For time-base alarms, currently, an `alarm.time.TimeAlarm()` is created.
//|
//|     If no alarms are specified, the microcontroller will deep sleep until reset.
//|
//|     :param circuitpython_typing.Alarm alarms: the alarms that can wake the microcontroller.
//|     :param Sequence[digitalio.DigitalInOut] preserve_dios: A sequence of `DigitalInOut` objects
//|       whose state should be preserved during deep sleep.
//|       If a `DigitalInOut` in the sequence is set to be an output,
//|       its current `DigitalInOut.value` (``True`` or ``False``)
//|       will be preserved during the deep sleep.
//|       If a `DigitalInOut` in the sequence is set to be an input,
//|       its current `DigitalInOut.pull` value (``DOWN``, ``UP``, or ``None``)
//|       will be preserved during deep sleep.
//|
//|     Preserving `DigitalInOut` states during deep sleep can be used to ensure that
//|     external or on-board devices are powered or unpowered during sleep, among other purposes.
//|
//|     On some microcontrollers, some pins cannot remain in their original state for hardware reasons.
//|
//|     **Limitations:** ``preserve_dios`` is currently only available on Espressif.
//|
//|     .. note::
//|       On Espressif chips, preserving pin settings during deep sleep may consume extra current.
//|       On ESP32, this was measured to be 250 uA or more.
//|       Consider not preserving pins unless you need to.
//|       Measure power consumption carefully both with no pins preserved and with the pins you might want to
//|       preserve to achieve the lowest consumption.
//|
//|     **If CircuitPython is connected to a host computer via USB or BLE
//|     the first time a deep sleep is requested,
//|     the connection will be maintained and the system will not go into deep sleep.**
//|     This allows the user to interrupt an existing program with ctrl-C,
//|     and to edit the files in CIRCUITPY, which would not be possible in true deep sleep.
//|
//|     If CircuitPython goes into a true deep sleep, and USB or BLE is reconnected,
//|     the next deep sleep will still be a true deep sleep. You must do a hard reset
//|     or power-cycle to exit a true deep sleep loop.
//|
//|     Here is a skeletal example:
//|
//|     .. code-block:: python
//|
//|         import alarm
//|         import time
//|         import board
//|
//|         print("Waking up")
//|
//|         # Create an alarm for 60 seconds from now, and also a pin alarm.
//|         time_alarm = alarm.time.TimeAlarm(monotonic_time=time.monotonic() + 60)
//|         pin_alarm = alarm.pin.PinAlarm(board.D7, False)
//|
//|         # Deep sleep until one of the alarm goes off. Then restart the program.
//|         alarm.exit_and_deep_sleep_until_alarms(time_alarm, pin_alarm)
//|     """
//|     ...
//|
static mp_obj_t alarm_exit_and_deep_sleep_until_alarms(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_preserve_dios };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_preserve_dios, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_empty_tuple} },
    };

    // args will contain only the value for preserve_dios. The *alarms args are in pos_args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(0, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    validate_objs_are_alarms(n_args, pos_args);

    mp_obj_t preserve_dios = args[ARG_preserve_dios].u_obj;
    const size_t num_dios = (size_t)MP_OBJ_SMALL_INT_VALUE(mp_obj_len(preserve_dios));
    digitalio_digitalinout_obj_t *dios_array[num_dios];

    for (mp_uint_t i = 0; i < num_dios; i++) {
        mp_obj_t dio = mp_obj_subscr(preserve_dios, MP_OBJ_NEW_SMALL_INT(i), MP_OBJ_SENTINEL);
        dios_array[i] = mp_arg_validate_type(dio, &digitalio_digitalinout_type, MP_QSTR_alarm);
    }

    common_hal_alarm_set_deep_sleep_alarms(n_args, pos_args, num_dios, dios_array);

    // Raise an exception, which will be processed in main.c.
    mp_raise_type_arg(&mp_type_DeepSleepRequest, NULL);

    // Doesn't get here.
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(alarm_exit_and_deep_sleep_until_alarms_obj, 0, alarm_exit_and_deep_sleep_until_alarms);

static const mp_map_elem_t alarm_pin_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_pin) },

    { MP_ROM_QSTR(MP_QSTR_PinAlarm), MP_OBJ_FROM_PTR(&alarm_pin_pinalarm_type) },
};

static MP_DEFINE_CONST_DICT(alarm_pin_globals, alarm_pin_globals_table);

static const mp_obj_module_t alarm_pin_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&alarm_pin_globals,
};

static const mp_map_elem_t alarm_time_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_time) },

    { MP_ROM_QSTR(MP_QSTR_TimeAlarm), MP_OBJ_FROM_PTR(&alarm_time_timealarm_type) },
};

static MP_DEFINE_CONST_DICT(alarm_time_globals, alarm_time_globals_table);

static const mp_obj_module_t alarm_time_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&alarm_time_globals,
};

#if CIRCUITPY_ALARM_TOUCH
static const mp_map_elem_t alarm_touch_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_touch) },
    { MP_ROM_QSTR(MP_QSTR_TouchAlarm), MP_OBJ_FROM_PTR(&alarm_touch_touchalarm_type) },
};

static MP_DEFINE_CONST_DICT(alarm_touch_globals, alarm_touch_globals_table);

static const mp_obj_module_t alarm_touch_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&alarm_touch_globals,
};
#endif

// The module table is mutable because .wake_alarm is a mutable attribute.
static mp_map_elem_t alarm_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_alarm) },

    // wake_alarm is a mutable attribute.
    { MP_ROM_QSTR(MP_QSTR_wake_alarm), mp_const_none },

    { MP_ROM_QSTR(MP_QSTR_light_sleep_until_alarms), MP_OBJ_FROM_PTR(&alarm_light_sleep_until_alarms_obj) },
    { MP_ROM_QSTR(MP_QSTR_exit_and_deep_sleep_until_alarms),
      MP_OBJ_FROM_PTR(&alarm_exit_and_deep_sleep_until_alarms_obj) },

    { MP_ROM_QSTR(MP_QSTR_pin), MP_OBJ_FROM_PTR(&alarm_pin_module) },
    { MP_ROM_QSTR(MP_QSTR_time), MP_OBJ_FROM_PTR(&alarm_time_module) },
    #if CIRCUITPY_ALARM_TOUCH
    { MP_ROM_QSTR(MP_QSTR_touch), MP_OBJ_FROM_PTR(&alarm_touch_module) },
    #endif

    { MP_ROM_QSTR(MP_QSTR_SleepMemory),   MP_OBJ_FROM_PTR(&alarm_sleep_memory_type) },
    { MP_ROM_QSTR(MP_QSTR_sleep_memory),  MP_OBJ_FROM_PTR(&alarm_sleep_memory_obj) },
};
static MP_DEFINE_MUTABLE_DICT(alarm_module_globals, alarm_module_globals_table);

// Fetch value from module dict.
mp_obj_t shared_alarm_get_wake_alarm(void) {
    mp_map_elem_t *elem =
        mp_map_lookup(&alarm_module_globals.map, MP_ROM_QSTR(MP_QSTR_wake_alarm), MP_MAP_LOOKUP);
    if (elem) {
        return elem->value;
    } else {
        return NULL;
    }
}

// Initialize .wake_alarm value.
void shared_alarm_save_wake_alarm(mp_obj_t alarm) {
    // Equivalent of:
    // alarm.wake_alarm = alarm
    mp_map_elem_t *elem =
        mp_map_lookup(&alarm_module_globals.map, MP_ROM_QSTR(MP_QSTR_wake_alarm), MP_MAP_LOOKUP);
    if (elem) {
        elem->value = alarm;
    }
}

const mp_obj_module_t alarm_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&alarm_module_globals,
};

extern void port_idle_until_interrupt(void);

MP_WEAK void common_hal_alarm_pretending_deep_sleep(void) {
    port_idle_until_interrupt();
}

MP_REGISTER_MODULE(MP_QSTR_alarm, alarm_module);
