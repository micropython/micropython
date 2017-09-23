/* Auto-generated config file hpl_usb_config.h */
#ifndef HPL_USB_CONFIG_H
#define HPL_USB_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

#define CONF_USB_N_0 0
#define CONF_USB_N_1 1
#define CONF_USB_N_2 2
#define CONF_USB_N_3 3
#define CONF_USB_N_4 4
#define CONF_USB_N_5 5
#define CONF_USB_N_6 6
#define CONF_USB_N_7 7
#define CONF_USB_N_8 8
#define CONF_USB_N_9 9
#define CONF_USB_N_10 10
#define CONF_USB_N_11 11
#define CONF_USB_N_12 12
#define CONF_USB_N_13 13
#define CONF_USB_N_14 14
#define CONF_USB_N_15 15

#define CONF_USB_D_EP_N_MAX (USB_EPT_NUM - 1)
#define CONF_USB_D_N_EP_MAX (CONF_USB_D_EP_N_MAX * 2 - 1)

// <h> USB Device HAL Configuration

// <y> Max number of endpoints supported
// <i> Limits the number of endpoints (described by EP address) can be used in app.
// <CONF_USB_N_1"> 1 (EP0 only)
// <CONF_USB_N_2"> 2 (EP0 + 1 endpoint)
// <CONF_USB_N_3"> 3 (EP0 + 2 endpoints)
// <CONF_USB_N_4"> 4 (EP0 + 3 endpoints)
// <CONF_USB_N_5"> 5 (EP0 + 4 endpoints)
// <CONF_USB_N_6"> 6 (EP0 + 5 endpoints)
// <CONF_USB_N_7"> 7 (EP0 + 6 endpoints)
// <CONF_USB_N_8"> 8 (EP0 + 7 endpoints)
// <CONF_USB_D_N_EP_MAX"> Max possible (by "Max Endpoint Number" config)
// <id> usbd_num_ep_sp
#ifndef CONF_USB_D_NUM_EP_SP
#define CONF_USB_D_NUM_EP_SP CONF_USB_N_4
#endif

// </h>

// <y> Max Endpoint Number supported
// <i> Limits the max endpoint number.
// <i> USB endpoint address is constructed by direction and endpoint number. Bit 8 of address set indicates the direction is IN. E.g., EP0x81 and EP0x01 have the same endpoint number, 1.
// <i> Reduce the value according to specific device design, to cut-off memory usage.
// <CONF_USB_N_0"> 0 (only EP0)
// <CONF_USB_N_1"> 1 (EP 0x81 or 0x01)
// <CONF_USB_N_2"> 2 (EP 0x82 or 0x02)
// <CONF_USB_N_3"> 3 (EP 0x83 or 0x03)
// <CONF_USB_N_4"> 4 (EP 0x84 or 0x04)
// <CONF_USB_N_5"> 5 (EP 0x85 or 0x05)
// <CONF_USB_N_6"> 6 (EP 0x86 or 0x06)
// <CONF_USB_N_7"> 7 (EP 0x87 or 0x07)
// <CONF_USB_EP_N_MAX"> Max possible (by HW)
// <i> The number of physical endpoints - 1
// <id> usbd_arch_max_ep_n
#ifndef CONF_USB_D_MAX_EP_N
#define CONF_USB_D_MAX_EP_N CONF_USB_N_2
#endif

// <y> USB Speed Limit
// <i> Limits the working speed of the device.
// <USB_SPEED_FS"> Full speed
// <USB_SPEED_LS"> Low Speed
// <id> usbd_arch_speed
#ifndef CONF_USB_D_SPEED
#define CONF_USB_D_SPEED USB_SPEED_FS
#endif

// <o> Cache buffer size for EP0
// <i> Cache is used because the USB hardware always uses DMA which requires specific memory feature.
// <i> EP0 is default control endpoint, so cache must be used to be able to receive SETUP packet at any time.
// <8=> Cached by 8 bytes buffer
// <16=> Cached by 16 bytes buffer
// <32=> Cached by 32 bytes buffer
// <64=> Cached by 64 bytes buffer
// <id> usb_arch_ep0_cache
#ifndef CONF_USB_EP0_CACHE
#define CONF_USB_EP0_CACHE 64
#endif

// <h> Cache configuration EP1
// <o> Cache buffer size for EP1 OUT
// <i> Cache is used because the USB hardware always uses DMA which requires specific memory feature.
// <i> This cache must be allocated if you plan to use the endpoint as control endpoint.
// <i> No cache means IN transaction not support data buffer outside of RAM, OUT transaction not support unaligned buffer and buffer size less than endpoint max packet size
// <0=> No cache
// <8=> Cached by 8 bytes buffer
// <16=> Cached by 16 bytes buffer
// <32=> Cached by 32 bytes buffer
// <64=> Cached by 64 bytes buffer
// <128=> Cached by 128 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <256=> Cached by 256 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <512=> Cached by 512 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <1024=> Cached by 1024 bytes buffer (interrupt or isochronous EP)
// <id> usb_arch_ep1_cache
#ifndef CONF_USB_EP1_CACHE
#define CONF_USB_EP1_CACHE 64
#endif

// <o> Cache buffer size for EP1 IN
// <i> Cache is used because the USB hardware always uses DMA which requires specific memory feature.
// <i> This cache must not be allocated if you plan to use the endpoint as control endpoint.
// <i> No cache means IN transaction not support data buffer outside of RAM, OUT transaction not support unaligned buffer and buffer size less than endpoint max packet size
// <0=> No cache
// <8=> Cached by 8 bytes buffer
// <16=> Cached by 16 bytes buffer
// <32=> Cached by 32 bytes buffer
// <64=> Cached by 64 bytes buffer
// <128=> Cached by 128 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <256=> Cached by 256 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <512=> Cached by 512 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <1024=> Cached by 1024 bytes buffer (interrupt or isochronous EP)
// <id> usb_ep1_I_CACHE
#ifndef CONF_USB_EP1_I_CACHE
#define CONF_USB_EP1_I_CACHE 0
#endif
// </h>

// <h> Cache configuration EP2
// <o> Cache buffer size for EP2 OUT
// <i> Cache is used because the USB hardware always uses DMA which requires specific memory feature.
// <i> This cache must be allocated if you plan to use the endpoint as control endpoint.
// <i> No cache means IN transaction not support data buffer outside of RAM, OUT transaction not support unaligned buffer and buffer size less than endpoint max packet size
// <0=> No cache
// <8=> Cached by 8 bytes buffer
// <16=> Cached by 16 bytes buffer
// <32=> Cached by 32 bytes buffer
// <64=> Cached by 64 bytes buffer
// <128=> Cached by 128 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <256=> Cached by 256 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <512=> Cached by 512 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <1024=> Cached by 1024 bytes buffer (interrupt or isochronous EP)
// <id> usb_arch_ep2_cache
#ifndef CONF_USB_EP2_CACHE
#define CONF_USB_EP2_CACHE 64
#endif

// <o> Cache buffer size for EP2 IN
// <i> Cache is used because the USB hardware always uses DMA which requires specific memory feature.
// <i> This cache must not be allocated if you plan to use the endpoint as control endpoint.
// <i> No cache means IN transaction not support data buffer outside of RAM, OUT transaction not support unaligned buffer and buffer size less than endpoint max packet size
// <0=> No cache
// <8=> Cached by 8 bytes buffer
// <16=> Cached by 16 bytes buffer
// <32=> Cached by 32 bytes buffer
// <64=> Cached by 64 bytes buffer
// <128=> Cached by 128 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <256=> Cached by 256 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <512=> Cached by 512 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <1024=> Cached by 1024 bytes buffer (interrupt or isochronous EP)
// <id> usb_ep2_I_CACHE
#ifndef CONF_USB_EP2_I_CACHE
#define CONF_USB_EP2_I_CACHE 0
#endif
// </h>

// <h> Cache configuration EP3
// <o> Cache buffer size for EP3 OUT
// <i> Cache is used because the USB hardware always uses DMA which requires specific memory feature.
// <i> This cache must be allocated if you plan to use the endpoint as control endpoint.
// <i> No cache means IN transaction not support data buffer outside of RAM, OUT transaction not support unaligned buffer and buffer size less than endpoint max packet size
// <0=> No cache
// <8=> Cached by 8 bytes buffer
// <16=> Cached by 16 bytes buffer
// <32=> Cached by 32 bytes buffer
// <64=> Cached by 64 bytes buffer
// <128=> Cached by 128 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <256=> Cached by 256 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <512=> Cached by 512 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <1024=> Cached by 1024 bytes buffer (interrupt or isochronous EP)
// <id> usb_arch_ep3_cache
#ifndef CONF_USB_EP3_CACHE
#define CONF_USB_EP3_CACHE 64
#endif

// <o> Cache buffer size for EP3 IN
// <i> Cache is used because the USB hardware always uses DMA which requires specific memory feature.
// <i> This cache must not be allocated if you plan to use the endpoint as control endpoint.
// <i> No cache means IN transaction not support data buffer outside of RAM, OUT transaction not support unaligned buffer and buffer size less than endpoint max packet size
// <0=> No cache
// <8=> Cached by 8 bytes buffer
// <16=> Cached by 16 bytes buffer
// <32=> Cached by 32 bytes buffer
// <64=> Cached by 64 bytes buffer
// <128=> Cached by 128 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <256=> Cached by 256 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <512=> Cached by 512 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <1024=> Cached by 1024 bytes buffer (interrupt or isochronous EP)
// <id> usb_ep3_I_CACHE
#ifndef CONF_USB_EP3_I_CACHE
#define CONF_USB_EP3_I_CACHE 0
#endif
// </h>

// <h> Cache configuration EP4
// <o> Cache buffer size for EP4 OUT
// <i> Cache is used because the USB hardware always uses DMA which requires specific memory feature.
// <i> This cache must be allocated if you plan to use the endpoint as control endpoint.
// <i> No cache means IN transaction not support data buffer outside of RAM, OUT transaction not support unaligned buffer and buffer size less than endpoint max packet size
// <0=> No cache
// <8=> Cached by 8 bytes buffer
// <16=> Cached by 16 bytes buffer
// <32=> Cached by 32 bytes buffer
// <64=> Cached by 64 bytes buffer
// <128=> Cached by 128 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <256=> Cached by 256 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <512=> Cached by 512 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <1024=> Cached by 1024 bytes buffer (interrupt or isochronous EP)
// <id> usb_arch_ep4_cache
#ifndef CONF_USB_EP4_CACHE
#define CONF_USB_EP4_CACHE 64
#endif

// <o> Cache buffer size for EP4 IN
// <i> Cache is used because the USB hardware always uses DMA which requires specific memory feature.
// <i> This cache must not be allocated if you plan to use the endpoint as control endpoint.
// <i> No cache means IN transaction not support data buffer outside of RAM, OUT transaction not support unaligned buffer and buffer size less than endpoint max packet size
// <0=> No cache
// <8=> Cached by 8 bytes buffer
// <16=> Cached by 16 bytes buffer
// <32=> Cached by 32 bytes buffer
// <64=> Cached by 64 bytes buffer
// <128=> Cached by 128 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <256=> Cached by 256 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <512=> Cached by 512 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <1024=> Cached by 1024 bytes buffer (interrupt or isochronous EP)
// <id> usb_ep4_I_CACHE
#ifndef CONF_USB_EP4_I_CACHE
#define CONF_USB_EP4_I_CACHE 0
#endif
// </h>

// <h> Cache configuration EP5
// <o> Cache buffer size for EP5 OUT
// <i> Cache is used because the USB hardware always uses DMA which requires specific memory feature.
// <i> This cache must be allocated if you plan to use the endpoint as control endpoint.
// <i> No cache means IN transaction not support data buffer outside of RAM, OUT transaction not support unaligned buffer and buffer size less than endpoint max packet size
// <0=> No cache
// <8=> Cached by 8 bytes buffer
// <16=> Cached by 16 bytes buffer
// <32=> Cached by 32 bytes buffer
// <64=> Cached by 64 bytes buffer
// <128=> Cached by 128 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <256=> Cached by 256 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <512=> Cached by 512 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <1024=> Cached by 1024 bytes buffer (interrupt or isochronous EP)
// <id> usb_arch_ep5_cache
#ifndef CONF_USB_EP5_CACHE
#define CONF_USB_EP5_CACHE 64
#endif

// <o> Cache buffer size for EP5 IN
// <i> Cache is used because the USB hardware always uses DMA which requires specific memory feature.
// <i> This cache must not be allocated if you plan to use the endpoint as control endpoint.
// <i> No cache means IN transaction not support data buffer outside of RAM, OUT transaction not support unaligned buffer and buffer size less than endpoint max packet size
// <0=> No cache
// <8=> Cached by 8 bytes buffer
// <16=> Cached by 16 bytes buffer
// <32=> Cached by 32 bytes buffer
// <64=> Cached by 64 bytes buffer
// <128=> Cached by 128 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <256=> Cached by 256 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <512=> Cached by 512 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <1024=> Cached by 1024 bytes buffer (interrupt or isochronous EP)
// <id> usb_ep5_I_CACHE
#ifndef CONF_USB_EP5_I_CACHE
#define CONF_USB_EP5_I_CACHE 0
#endif
// </h>

// <h> Cache configuration EP6
// <o> Cache buffer size for EP6 OUT
// <i> Cache is used because the USB hardware always uses DMA which requires specific memory feature.
// <i> This cache must be allocated if you plan to use the endpoint as control endpoint.
// <i> No cache means IN transaction not support data buffer outside of RAM, OUT transaction not support unaligned buffer and buffer size less than endpoint max packet size
// <0=> No cache
// <8=> Cached by 8 bytes buffer
// <16=> Cached by 16 bytes buffer
// <32=> Cached by 32 bytes buffer
// <64=> Cached by 64 bytes buffer
// <128=> Cached by 128 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <256=> Cached by 256 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <512=> Cached by 512 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <1024=> Cached by 1024 bytes buffer (interrupt or isochronous EP)
// <id> usb_arch_ep6_cache
#ifndef CONF_USB_EP6_CACHE
#define CONF_USB_EP6_CACHE 64
#endif

// <o> Cache buffer size for EP6 IN
// <i> Cache is used because the USB hardware always uses DMA which requires specific memory feature.
// <i> This cache must not be allocated if you plan to use the endpoint as control endpoint.
// <i> No cache means IN transaction not support data buffer outside of RAM, OUT transaction not support unaligned buffer and buffer size less than endpoint max packet size
// <0=> No cache
// <8=> Cached by 8 bytes buffer
// <16=> Cached by 16 bytes buffer
// <32=> Cached by 32 bytes buffer
// <64=> Cached by 64 bytes buffer
// <128=> Cached by 128 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <256=> Cached by 256 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <512=> Cached by 512 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <1024=> Cached by 1024 bytes buffer (interrupt or isochronous EP)
// <id> usb_ep6_I_CACHE
#ifndef CONF_USB_EP6_I_CACHE
#define CONF_USB_EP6_I_CACHE 0
#endif
// </h>

// <h> Cache configuration EP7
// <o> Cache buffer size for EP7 OUT
// <i> Cache is used because the USB hardware always uses DMA which requires specific memory feature.
// <i> This cache must be allocated if you plan to use the endpoint as control endpoint.
// <i> No cache means IN transaction not support data buffer outside of RAM, OUT transaction not support unaligned buffer and buffer size less than endpoint max packet size
// <0=> No cache
// <8=> Cached by 8 bytes buffer
// <16=> Cached by 16 bytes buffer
// <32=> Cached by 32 bytes buffer
// <64=> Cached by 64 bytes buffer
// <128=> Cached by 128 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <256=> Cached by 256 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <512=> Cached by 512 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <1024=> Cached by 1024 bytes buffer (interrupt or isochronous EP)
// <id> usb_arch_ep7_cache
#ifndef CONF_USB_EP7_CACHE
#define CONF_USB_EP7_CACHE 64
#endif

// <o> Cache buffer size for EP7 IN
// <i> Cache is used because the USB hardware always uses DMA which requires specific memory feature.
// <i> This cache must not be allocated if you plan to use the endpoint as control endpoint.
// <i> No cache means IN transaction not support data buffer outside of RAM, OUT transaction not support unaligned buffer and buffer size less than endpoint max packet size
// <0=> No cache
// <8=> Cached by 8 bytes buffer
// <16=> Cached by 16 bytes buffer
// <32=> Cached by 32 bytes buffer
// <64=> Cached by 64 bytes buffer
// <128=> Cached by 128 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <256=> Cached by 256 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <512=> Cached by 512 bytes buffer (HS Bulk or interrupt or isochronous EP)
// <1024=> Cached by 1024 bytes buffer (interrupt or isochronous EP)
// <id> usb_ep7_I_CACHE
#ifndef CONF_USB_EP7_I_CACHE
#define CONF_USB_EP7_I_CACHE 0
#endif
// </h>

// <<< end of configuration section >>>

#endif // HPL_USB_CONFIG_H
