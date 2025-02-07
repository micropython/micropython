

#include "py/runtime.h"
#include "py/mphal.h"

#if MICROPY_HW_USB_NET

#include "tusb.h"
// #include "lwip/ethip6.h"
// #include "lwip/init.h"
// #include "lwip/timeouts.h"
#include "shared/netutils/netutils.h"
#include "extmod/modnetwork.h"

#include "lwip/etharp.h"
#include "lwip/dns.h"
#include "lwip/dhcp.h"
#include "netif/ethernet.h"

// typedef struct _eth_t {
//     uint32_t trace_flags;
//     struct netif netif_data;
//     struct dhcp dhcp_struct;
// } eth_t;

#define INIT_IP4(a, b, c, d) \
  { PP_HTONL(LWIP_MAKEU32(a, b, c, d)) }

/* lwip context */
static struct netif netif_data;

/* shared between tud_network_recv_cb() and service_traffic() */
static struct pbuf *received_frame;

/* this is used by this code, ./class/net/net_driver.c, and usb_descriptors.c */
/* ideally speaking, this should be generated from the hardware's unique ID (if available) */
/* it is suggested that the first byte is 0x02 to indicate a link-local address */
uint8_t tud_network_mac_address[6] = {0x02, 0x02, 0x84, 0x6A, 0x96, 0x00};


static err_t linkoutput_fn(struct netif *netif, struct pbuf *p) {
  (void) netif;

  for (;;) {
    /* if TinyUSB isn't ready, we must signal back to lwip that there is nothing we can do */
    if (!tud_ready())
      return ERR_USE;

    /* if the network driver can accept another packet, we make it happen */
    if (tud_network_can_xmit(p->tot_len)) {
      tud_network_xmit(p, 0 /* unused for this example */);
      return ERR_OK;
    }

    /* transfer execution to TinyUSB in the hopes that it will finish transmitting the prior packet */
    tud_task();
  }
}

static err_t ip4_output_fn(struct netif *netif, struct pbuf *p, const ip4_addr_t *addr) {
  return etharp_output(netif, p, addr);
}

#if LWIP_IPV6
static err_t ip6_output_fn(struct netif *netif, struct pbuf *p, const ip6_addr_t *addr) {
  return ethip6_output(netif, p, addr);
}
#endif

static err_t netif_init_cb(struct netif *netif) {
  LWIP_ASSERT("netif != NULL", (netif != NULL));
  netif->mtu = CFG_TUD_NET_MTU;
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_NETARP | NETIF_FLAG_LINK_UP | NETIF_FLAG_UP;
  netif->state = NULL;
  netif->name[0] = 'E';
  netif->name[1] = 'X';
  netif->linkoutput = linkoutput_fn;
  netif->output = ip4_output_fn;
#if LWIP_IPV6
  netif->output_ip6 = ip6_output_fn;
#endif
  return ERR_OK;
}


bool tud_network_recv_cb(const uint8_t *src, uint16_t size) {
  /* this shouldn't happen, but if we get another packet before
  parsing the previous, we must signal our inability to accept it */
  if (received_frame) return false;

  if (size) {
    struct pbuf *p = pbuf_alloc(PBUF_RAW, size, PBUF_POOL);

    if (p) {
      /* pbuf_alloc() has already initialized struct; all we need to do is copy the data */
      memcpy(p->payload, src, size);

      /* store away the pointer for service_traffic() to later handle */
      received_frame = p;
    }
  }

  return true;
}

uint16_t tud_network_xmit_cb(uint8_t *dst, void *ref, uint16_t arg) {
  struct pbuf *p = (struct pbuf *) ref;

  (void) arg; /* unused for this example */

  return pbuf_copy_partial(p, dst, p->tot_len, 0);
}

static void service_traffic(void) {
  /* handle any packet received by tud_network_recv_cb() */
  if (received_frame) {
    ethernet_input(received_frame, &netif_data);
    pbuf_free(received_frame);
    received_frame = NULL;
    tud_network_recv_renew();
  }

  sys_check_timeouts();
}

void tud_network_init_cb(void) {
  /* if the network is re-initializing and we have a leftover packet, we must do a cleanup */
  if (received_frame) {
    pbuf_free(received_frame);
    received_frame = NULL;
  }
}

void mp_usbd_net_init(void) {
    MICROPY_PY_LWIP_ENTER
    netif->hwaddr_len = sizeof(tud_network_mac_address);
    memcpy(netif->hwaddr, tud_network_mac_address, sizeof(tud_network_mac_address));
    netif->hwaddr[5] ^= 0x01;

    netif = netif_add(netif, &ipaddr, &netmask, &gateway, NULL, netif_init_cb, ip_input);
    #if LWIP_IPV6
    netif_create_ip6_linklocal_address(netif, 1);
    #endif
    netif_set_default(netif);
    MICROPY_PY_LWIP_EXIT

}

#endif