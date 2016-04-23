#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t HAL_NET_SetNetWatchDog(uint32_t timeOutInuS);

/**
 * Notification that the wifi network has been connected to.
 */
void HAL_NET_notify_connected();
void HAL_NET_notify_disconnected();

/**
 * Notification that an IP address has been received via DHCP.
 * todo - what with the case of static IP config?
 */
void HAL_NET_notify_dhcp(bool dhcp);

void HAL_NET_notify_can_shutdown();


#ifdef __cplusplus
}
#endif
