/**
 ******************************************************************************
 * @file    spark_wiring_system.h
 * @author  Satish Nair, Zachary Crockett, Matthew McGowan
 ******************************************************************************
  Copyright (c) 2013-2015 Particle Industries, Inc.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
  ******************************************************************************
 */

#ifndef SPARK_WIRING_SYSTEM_H
#define	SPARK_WIRING_SYSTEM_H
#include "spark_wiring_ticks.h"
#include "spark_wiring_string.h"
#include "spark_wiring_version.h"
#include "system_mode.h"
#include "system_update.h"
#include "system_sleep.h"
#include "system_cloud.h"
#include "system_event.h"
#include "core_hal.h"
#include "interrupts_hal.h"
#include "core_hal.h"
#include "system_user.h"
#include "system_version.h"

#if defined(SPARK_PLATFORM) && PLATFORM_ID!=3
#define SYSTEM_HW_TICKS 1
#else
#define SYSTEM_HW_TICKS 0
#endif

#if SYSTEM_HW_TICKS
#include "hw_ticks.h"
#endif

class Stream;

class SystemClass {
public:

    SystemClass(System_Mode_TypeDef mode = DEFAULT) {
        set_system_mode(mode);
    }

    static System_Mode_TypeDef mode(void) {
        return system_mode();
    }

    static bool firmwareUpdate(Stream *serialObj) {
        return system_firmwareUpdate(serialObj);
    }

    static void factoryReset(void);
    static void dfu(bool persist=false);
    static void reset(void);

    static void enterSafeMode(void) {
        HAL_Core_Enter_Safe_Mode(NULL);
    }

#if SYSTEM_HW_TICKS
    static inline uint32_t ticksPerMicrosecond()
    {
        return SYSTEM_US_TICKS;
    }

    static inline uint32_t ticks()
    {
        return SYSTEM_TICK_COUNTER;
    }

    static inline void ticksDelay(uint32_t duration)
    {
        uint32_t start = ticks();
        while ((ticks()-start)<duration) {}
    }
#endif

    static void sleep(Spark_Sleep_TypeDef sleepMode, long seconds=0);
    static void sleep(long seconds) { sleep(SLEEP_MODE_WLAN, seconds); }
    static void sleep(uint16_t wakeUpPin, InterruptMode edgeTriggerMode, long seconds=0);
    static String deviceID(void) { return spark_deviceID(); }

    static uint16_t buttonPushed(uint8_t button=0) {
        return system_button_pushed_duration(button, NULL);
    }

    static bool on(system_event_t events, void(*handler)(system_event_t, int,void*)) {
        return !system_subscribe_event(events, reinterpret_cast<system_event_handler_t*>(handler), nullptr);
    }

    static bool on(system_event_t events, void(*handler)(system_event_t, int)) {
        return system_subscribe_event(events, reinterpret_cast<system_event_handler_t*>(handler), NULL);
    }

    static bool on(system_event_t events, void(*handler)(system_event_t)) {
        return system_subscribe_event(events, reinterpret_cast<system_event_handler_t*>(handler), NULL);
    }

    static bool on(system_event_t events, void(*handler)()) {
        return system_subscribe_event(events, reinterpret_cast<system_event_handler_t*>(handler), NULL);
    }

    static void off(void(*handler)(system_event_t, int,void*)) {
        system_unsubscribe_event(all_events, handler, nullptr);
    }

    static void off(system_event_t events, void(*handler)(system_event_t, int,void*)) {
        system_unsubscribe_event(events, handler, nullptr);
    }


    static uint32_t freeMemory();

    template<typename Condition, typename While> static bool waitConditionWhile(Condition _condition, While _while) {
        while (_while() && !_condition()) {
            spark_process();
        }
        return _condition();
    }

    template<typename Condition> static bool waitCondition(Condition _condition) {
        return waitConditionWhile(_condition, []{ return true; });
    }

    template<typename Condition> static bool waitCondition(Condition _condition, system_tick_t timeout) {
        const system_tick_t start = millis();
        return waitConditionWhile(_condition, [=]{ return (millis()-start)<timeout; });
    }

    bool set(hal_system_config_t config_type, const void* data, unsigned length)
    {
        return HAL_Set_System_Config(config_type, data, length)>=0;
    }

    bool set(hal_system_config_t config_type, const char* data)
    {
        return set(config_type, data, strlen(data));
    }


    inline bool featureEnabled(HAL_Feature feature)
    {
        if (feature==FEATURE_WARM_START)
            return __backup_ram_was_valid();
        return HAL_Feature_Get(feature);
    }

    inline int enableFeature(HAL_Feature feature)
    {
        return HAL_Feature_Set(feature, true);
    }

    inline int disableFeature(HAL_Feature feature)
    {
        return HAL_Feature_Set(feature, false);
    }

    String version()
    {
        SystemVersionInfo info;
        system_version_info(&info, nullptr);
        return String(info.versionString);
    }

    uint32_t versionNumber()
    {
        SystemVersionInfo info;
        system_version_info(&info, nullptr);
        return info.versionNumber;
    }

    inline void enableUpdates()
    {
        set_flag(SYSTEM_FLAG_OTA_UPDATE_ENABLED, true);
    }

    inline void disableUpdates()
    {
        set_flag(SYSTEM_FLAG_OTA_UPDATE_ENABLED, false);
    }

    inline uint8_t updatesPending()
    {
        return get_flag(SYSTEM_FLAG_OTA_UPDATE_PENDING)!=0;
    }

    inline uint8_t updatesEnabled()
    {
        return get_flag(SYSTEM_FLAG_OTA_UPDATE_ENABLED)!=0;
    }


    inline void enableReset()
    {
        set_flag(SYSTEM_FLAG_RESET_ENABLED, true);
    }

    inline void disableReset()
    {
        set_flag(SYSTEM_FLAG_RESET_ENABLED, false);
    }

    inline uint8_t resetEnabled()
    {
        return get_flag(SYSTEM_FLAG_RESET_ENABLED)!=0;
    }

    inline uint8_t resetPending()
    {
        return get_flag(SYSTEM_FLAG_RESET_PENDING)!=0;
    }


private:

    inline uint8_t get_flag(system_flag_t flag)
    {
        uint8_t value = 0;
        system_get_flag(flag, &value, nullptr);
        return value;
    }

    inline void set_flag(system_flag_t flag, uint8_t value)
    {
        system_set_flag(flag, value, nullptr);
    }


};

extern SystemClass System;

#define SYSTEM_MODE(mode)  SystemClass SystemMode(mode);

#define SYSTEM_THREAD(state) STARTUP(system_thread_set_state(spark::feature::state, NULL));

#define waitFor(condition, timeout) System.waitCondition([]{ return (condition)(); }, (timeout))
#define waitUntil(condition) System.waitCondition([]{ return (condition)(); })

#endif	/* SPARK_WIRING_SYSTEM_H */

