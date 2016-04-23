/**
 ******************************************************************************
 * @file    spark_wiring_cloud.h
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

#pragma once

#include "spark_wiring_string.h"
#include "events.h"
#include "system_cloud.h"
#include "system_sleep.h"
#include "spark_protocol_functions.h"
#include "spark_wiring_system.h"
#include "spark_wiring_watchdog.h"
#include "interrupts_hal.h"
#include <functional>

typedef std::function<user_function_int_str_t> user_std_function_int_str_t;
typedef std::function<void (const char*, const char*)> wiring_event_handler_t;

#ifdef SPARK_NO_CLOUD
#define CLOUD_FN(x,y) (y)
#else
#define CLOUD_FN(x,y) (x)
#endif

class PublishFlag
{
public:
	typedef uint8_t flag_t;
	PublishFlag(flag_t flag) : flag_(flag) {}

	explicit operator flag_t() const { return flag_; }

	flag_t flag() const { return flag_; }

private:
	flag_t flag_;


};

const PublishFlag PUBLIC(PUBLISH_EVENT_FLAG_PUBLIC);
const PublishFlag PRIVATE(PUBLISH_EVENT_FLAG_PRIVATE);
const PublishFlag NO_ACK(PUBLISH_EVENT_FLAG_NO_ACK);


class CloudClass {


public:


    static inline bool variable(const char* varKey, const bool& var)
    {
        return variable(varKey, &var, BOOLEAN);
    }

    static inline bool variable(const char* varKey, const int& var)
    {
        return variable(varKey, &var, INT);
    }
#if PLATFORM_ID!=3
    // compiling with gcc this function duplicates the previous one.
    static inline bool variable(const char* varKey, const int32_t& var)
    {
        return variable(varKey, &var, INT);
    }
#endif
    static inline bool variable(const char* varKey, const uint32_t& var)
    {
        return variable(varKey, &var, INT);
    }

#if PLATFORM_ID!=3
    static bool variable(const char* varKey, const float& var)
    __attribute__((error("Please change the variable from type `float` to `double` for use with Particle.variable().")));
#endif

    static inline bool variable(const char* varKey, const double& var)
    {
        return variable(varKey, &var, DOUBLE);
    }

    static inline bool variable(const char* varKey, const String& var)
    {
        return variable(varKey, &var, STRING);
    }

    static inline bool variable(const char* varKey, const char* var)
    {
        return variable(varKey, var, STRING);
    }

    template<std::size_t N>
    static inline bool variable(const char* varKey, const char var[N])
    {
        return variable(varKey, var, STRING);
    }

    template<std::size_t N>
    static inline bool variable(const char* varKey, const unsigned char var[N])
    {
        return variable(varKey, var, STRING);
    }

    static inline bool variable(const char *varKey, const uint8_t* userVar, const CloudVariableTypeString& userVarType)
    {
        return variable(varKey, (const char*)userVar, userVarType);
    }

    template<typename T> static inline bool variable(const char *varKey, const typename T::varref userVar, const T& userVarType)
    {
        return CLOUD_FN(spark_variable(varKey, (const void*)userVar, T::value(), NULL), false);
    }

    static inline bool variable(const char *varKey, const int32_t* userVar, const CloudVariableTypeInt& userVarType)
    {
        return CLOUD_FN(spark_variable(varKey, (const void*)userVar, CloudVariableTypeInt::value(), NULL), false);
    }

    static inline bool variable(const char *varKey, const uint32_t* userVar, const CloudVariableTypeInt& userVarType)
    {
        return CLOUD_FN(spark_variable(varKey, (const void*)userVar, CloudVariableTypeInt::value(), NULL), false);
    }

    // Return clear errors for common misuses of Particle.variable()
    template<typename T, std::size_t N>
    static inline bool variable(const char *varKey, const T (*userVar)[N], const CloudVariableTypeString& userVarType)
    {
        static_assert(sizeof(T)==0, "\n\nUse Particle.variable(\"name\", myVar, STRING); without & in front of myVar\n\n");
        return false;
    }

    template<typename T>
    static inline bool variable(const T *varKey, const String *userVar, const CloudVariableTypeString& userVarType)
    {
        spark_variable_t extra;
        extra.size = sizeof(extra);
        extra.update = update_string_variable;
        return CLOUD_FN(spark_variable(varKey, userVar, CloudVariableTypeString::value(), &extra), false);
    }

    template<typename T>
    static inline bool variable(const T *varKey, const String &userVar, const CloudVariableTypeString& userVarType)
    {
        static_assert(sizeof(T)==0, "\n\nIn Particle.variable(\"name\", myVar, STRING); myVar must be declared as char myVar[] not String myVar\n\n");
        return false;
    }

    static bool function(const char *funcKey, user_function_int_str_t* func)
    {
        return CLOUD_FN(register_function(call_raw_user_function, (void*)func, funcKey), false);
    }

    static bool function(const char *funcKey, user_std_function_int_str_t func, void* reserved=NULL)
    {
#ifdef SPARK_NO_CLOUD
        return false;
#else
        bool success = false;
        if (func) // if the call-wrapper has wrapped a callable object
        {
            auto wrapper = new user_std_function_int_str_t(func);
            if (wrapper) {
                success = register_function(call_std_user_function, wrapper, funcKey);
            }
        }
        return success;
#endif
    }

    template <typename T>
    static void function(const char *funcKey, int (T::*func)(String), T *instance) {
      using namespace std::placeholders;
      function(funcKey, std::bind(func, instance, _1));
    }

    inline bool publish(const char *eventName, PublishFlag eventType=PUBLIC)
    {
        return CLOUD_FN(spark_send_event(eventName, NULL, 60, PublishFlag::flag_t(eventType), NULL), false);
    }

    inline bool publish(const char *eventName, const char *eventData, PublishFlag eventType=PUBLIC)
    {
        return CLOUD_FN(spark_send_event(eventName, eventData, 60, PublishFlag::flag_t(eventType), NULL), false);
    }

    inline bool publish(const char *eventName, const char *eventData, PublishFlag f1, PublishFlag f2)
    {
        return CLOUD_FN(spark_send_event(eventName, eventData, 60, f1.flag()+f2.flag(), NULL), false);
    }


    inline bool publish(const char *eventName, const char *eventData, int ttl, PublishFlag eventType=PUBLIC)
    {
        return CLOUD_FN(spark_send_event(eventName, eventData, ttl, PublishFlag::flag_t(eventType), NULL), false);
    }

    inline bool subscribe(const char *eventName, EventHandler handler, Spark_Subscription_Scope_TypeDef scope=ALL_DEVICES)
    {
        return CLOUD_FN(spark_subscribe(eventName, handler, NULL, scope, NULL, NULL), false);
    }

    inline bool subscribe(const char *eventName, EventHandler handler, const char *deviceID)
    {
        return CLOUD_FN(spark_subscribe(eventName, handler, NULL, MY_DEVICES, deviceID, NULL), false);
    }

    bool subscribe(const char *eventName, wiring_event_handler_t handler, Spark_Subscription_Scope_TypeDef scope=ALL_DEVICES)
    {
        return subscribe_wiring(eventName, handler, scope);
    }

    bool subscribe(const char *eventName, wiring_event_handler_t handler, const char *deviceID)
    {
        return subscribe_wiring(eventName, handler, MY_DEVICES, deviceID);
    }

    template <typename T>
    bool subscribe(const char *eventName, void (T::*handler)(const char *, const char *), T *instance, Spark_Subscription_Scope_TypeDef scope=ALL_DEVICES)
    {
        using namespace std::placeholders;
        return subscribe(eventName, std::bind(handler, instance, _1, _2), scope);
    }

    template <typename T>
    bool subscribe(const char *eventName, void (T::*handler)(const char *, const char *), T *instance, const char *deviceID)
    {
        using namespace std::placeholders;
        return subscribe(eventName, std::bind(handler, instance, _1, _2), deviceID);
    }

    void unsubscribe()
    {
        CLOUD_FN(spark_protocol_remove_event_handlers(sp(), NULL), (void)0);
    }

    bool syncTime(void)
    {
        return CLOUD_FN(spark_protocol_send_time_request(sp()),false);
    }

    static void sleep(long seconds) __attribute__ ((deprecated("Please use System.sleep() instead.")))
    { SystemClass::sleep(seconds); }
    static void sleep(Spark_Sleep_TypeDef sleepMode, long seconds=0) __attribute__ ((deprecated("Please use System.sleep() instead.")))
    { SystemClass::sleep(sleepMode, seconds); }
    static void sleep(uint16_t wakeUpPin, InterruptMode edgeTriggerMode, long seconds=0) __attribute__ ((deprecated("Please use System.sleep() instead.")))
    { SystemClass::sleep(wakeUpPin, edgeTriggerMode, seconds); }

    static bool connected(void) { return spark_connected(); }
    static bool disconnected(void) { return !connected(); }
    static void connect(void) { spark_connect(); }
    static void disconnect(void) { spark_disconnect(); }
    static void process(void) {
    		application_checkin();
    		spark_process();
    }
    static String deviceID(void) { return SystemClass::deviceID(); }

private:

    static bool register_function(cloud_function_t fn, void* data, const char* funcKey);
    static int call_raw_user_function(void* data, const char* param, void* reserved);
    static int call_std_user_function(void* data, const char* param, void* reserved);

    static void call_wiring_event_handler(const void* param, const char *event_name, const char *data);

    ProtocolFacade* sp() { return spark_protocol_instance(); }

    bool subscribe_wiring(const char *eventName, wiring_event_handler_t handler, Spark_Subscription_Scope_TypeDef scope, const char *deviceID = NULL)
    {
#ifdef SPARK_NO_CLOUD
        return false;
#else
        bool success = false;
        if (handler) // if the call-wrapper has wrapped a callable object
        {
            auto wrapper = new wiring_event_handler_t(handler);
            if (wrapper) {
                success = spark_subscribe(eventName, (EventHandler)call_wiring_event_handler, wrapper, scope, deviceID, NULL);
            }
        }
        return success;
#endif
    }

    static const void* update_string_variable(const char* name, Spark_Data_TypeDef type, const void* var, void* reserved)
    {
        const String* s = (const String*)var;
        return s->c_str();
    }
};


extern CloudClass Spark __attribute__((deprecated("Spark is now Particle.")));
extern CloudClass Particle;
