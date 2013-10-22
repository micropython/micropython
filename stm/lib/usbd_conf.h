#ifndef __USBD_CONF__H__
#define __USBD_CONF__H__

#define USBD_CFG_MAX_NUM            1
#define USBD_ITF_MAX_NUM            1 // TODO need more than 1?

// CDC Endpoints parameters
#define CDC_IN_EP                   0x81  /* EP1 for data IN */
#define CDC_OUT_EP                  0x01  /* EP1 for data OUT */
#define CDC_CMD_EP                  0x82  /* EP2 for CDC commands */
#define CDC_DATA_MAX_PACKET_SIZE    64   /* Endpoint IN & OUT Packet size */
#define CDC_CMD_PACKET_SZE          8    /* Control Endpoint Packet size */
#define CDC_IN_FRAME_INTERVAL       5    /* Number of frames between IN transfers */
#define APP_RX_DATA_SIZE            2048 /* Total size of IN buffer: 
                                                APP_RX_DATA_SIZE*8/MAX_BAUDARATE*1000 should be > CDC_IN_FRAME_INTERVAL */

// MSC parameters
#define MSC_IN_EP                   0x83
#define MSC_OUT_EP                  0x03
#define MSC_MAX_PACKET              64
#define MSC_MEDIA_PACKET            2048 /* XXX was 4096; how small can we make it? */

#endif //__USBD_CONF__H__
