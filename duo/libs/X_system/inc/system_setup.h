/**
  ******************************************************************************
  * @file    wifi_credentials_reader.h
  * @author  Zachary Crockett and Satish Nair
  * @version V1.0.0
  * @date    24-April-2013
  * @brief   header for wifi_credentials_reader.cpp
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

#include <string.h>
#include "spark_wiring_usbserial.h"
#include "spark_wiring_platform.h"

#if PLATFORM_ID>2
#define SETUP_OVER_SERIAL1 1
#endif

#ifndef SETUP_OVER_SERIAL1
#define SETUP_OVER_SERIAL1 0
#endif

typedef int (*ConnectCallback)( void* data,
                                const char *ssid,
                                const char *password,
                                unsigned long security_type,
                                unsigned long cipher,
                                bool dry_run);

class WiFiTester;

struct SystemSetupConsoleConfig
{

};


#if Wiring_WiFi
struct WiFiSetupConsoleConfig : SystemSetupConsoleConfig
{
    ConnectCallback connect_callback;
    void* connect_callback_data;
};
#endif

template<typename Config> class SystemSetupConsole
{
public:
    SystemSetupConsole(Config& config);
    ~SystemSetupConsole() = default;
    virtual void loop(void);
protected:
    virtual void exit()=0;
    virtual void handle(char c);
    Config& config;
    void print(const char *s);
    void read_line(char *dst, int max_len);

private:
    USBSerial serial;

};

#if Wiring_WiFi
class WiFiSetupConsole : public SystemSetupConsole<WiFiSetupConsoleConfig>
{
    using super = SystemSetupConsole<WiFiSetupConsoleConfig>;

public:
    WiFiSetupConsole(WiFiSetupConsoleConfig& config);
    ~WiFiSetupConsole();
    virtual void loop() override;

protected:
    virtual void handle(char c) override;
    virtual void exit() override;
private:
#if SETUP_OVER_SERIAL1
    bool serial1Enabled;
    uint8_t magicPos;                   // how far long the magic key we are
    WiFiTester* tester;
#endif
    char ssid[33];
    char password[65];
    char security_type_string[2];
};
#endif

#if Wiring_Cellular
class CellularSetupConsoleConfig : SystemSetupConsoleConfig
{

};

class CellularSetupConsole : public SystemSetupConsole<CellularSetupConsoleConfig>
{
    using super = SystemSetupConsole<CellularSetupConsoleConfig>;

public:
    CellularSetupConsole(CellularSetupConsoleConfig& config);
    ~CellularSetupConsole() = default;

    virtual void exit() override;
    virtual void handle(char c) override;
};

#endif
