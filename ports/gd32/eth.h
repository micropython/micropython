// eth.h - GD32F4 ENET driver interface.
#ifndef MICROPY_INCLUDED_GD32_ETH_H
#define MICROPY_INCLUDED_GD32_ETH_H

#include <stdbool.h>
#include <stdint.h>

#include "py/mpconfig.h"

#if MICROPY_PY_LWIP
#include "lwip/netif.h"
#endif

// Driver instance.  Exposed so the network.LAN object can hold one statically.
typedef struct _eth_t {
    #if MICROPY_PY_LWIP
    struct netif netif;
    #endif
    uint8_t mac[6];
    uint32_t phy_addr;
    bool initialized;
} eth_t;

// Initialise the MAC, PHY and DMA rings.  Returns 0 on success, a negative
// value when the peripheral or the PHY could not be brought up.
int eth_init(eth_t *self, uint32_t phy_addr);

// Current link state as reported by the PHY status register.
bool eth_link_status(eth_t *self);

// Override the MAC address used for the netif (call before eth_init).
void eth_set_mac(eth_t *self, const uint8_t *mac);

#if MICROPY_PY_LWIP
// The lwIP interface backing this driver.
struct netif *eth_netif(eth_t *self);
#endif

#endif // MICROPY_INCLUDED_GD32_ETH_H
