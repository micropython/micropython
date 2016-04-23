#pragma once

#include "inet_hal.h"
#include "wiced.h"

#ifdef __cplusplus
inline wiced_interface_t wiced_wlan_interface(network_interface_t nif)
{
    return nif ? WICED_AP_INTERFACE : WICED_STA_INTERFACE;
}
#else
    #define wiced_wlan_interface(nif) (nif ? WICED_AP_INTERFACE : WICED_STA_INTERFACE)
#endif