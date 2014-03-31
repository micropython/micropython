#ifndef _USB_CDC_MSC_CORE_H_
#define _USB_CDC_MSC_CORE_H_

#include  "usbd_msc_bot.h"
#include  "usbd_msc_scsi.h"
#include  "usbd_ioreq.h"

// CDC and MSC packet sizes
#define CDC_DATA_FS_MAX_PACKET_SIZE (64) // endpoint IN & OUT packet size
#define MSC_MEDIA_PACKET            (2048) // was 8192; how low can it go whilst still working?

#if 0
// CDC
#define USB_CDC_MSC_CONFIG_DESC_SIZ (75)
#define NUM_INTERFACES (2)
#define USE_CDC (1)
#define USE_MSC (0)
#define CDC_IN_EP                       0x81  /* EP1 for data IN */
#define CDC_OUT_EP                      0x01  /* EP1 for data OUT */
#define CDC_CMD_EP                      0x82  /* EP2 for CDC commands */
#define MSC_IN_EP (0x83)
#define MSC_OUT_EP (0x03)
#elif 0
// MSC
#define USB_CDC_MSC_CONFIG_DESC_SIZ (32)
#define NUM_INTERFACES (1)
#define USE_CDC (0)
#define USE_MSC (1)
#define CDC_IN_EP                       0x81  /* EP1 for data IN */
#define CDC_OUT_EP                      0x01  /* EP1 for data OUT */
#define CDC_CMD_EP                      0x82  /* EP2 for CDC commands */
#define MSC_IFACE_NUM (0)
#define MSC_IN_EP (0x81)
#define MSC_OUT_EP (0x01)
#elif 1
// CDC + MSC
#define USB_CDC_MSC_CONFIG_DESC_SIZ (98)
#define NUM_INTERFACES (3)
#define USE_CDC (1)
#define USE_MSC (1)
#define CDC_IFACE_NUM (0)
#define CDC_IN_EP                       0x81  /* EP1 for data IN */
#define CDC_OUT_EP                      0x01  /* EP1 for data OUT */
#define CDC_CMD_EP                      0x82  /* EP2 for CDC commands */
#define MSC_IFACE_NUM (2)
#define MSC_IN_EP (0x83)
#define MSC_OUT_EP (0x03)
#else
// MSC + CDC
#define USB_CDC_MSC_CONFIG_DESC_SIZ (98)
#define NUM_INTERFACES (3)
#define USE_CDC (1)
#define USE_MSC (1)
#define CDC_IFACE_NUM (1)
#define CDC_IN_EP                       0x83  /* EP1 for data IN */
#define CDC_OUT_EP                      0x03  /* EP1 for data OUT */
#define CDC_CMD_EP                      0x82  /* EP2 for CDC commands */
#define MSC_IFACE_NUM (0)
#define MSC_IN_EP (0x81)
#define MSC_OUT_EP (0x01)
#endif

typedef struct {
  uint32_t bitrate;
  uint8_t  format;
  uint8_t  paritytype;
  uint8_t  datatype;
} USBD_CDC_LineCodingTypeDef;

typedef struct _USBD_CDC_Itf {
  int8_t (* Init)          (void);
  int8_t (* DeInit)        (void);
  int8_t (* Control)       (uint8_t, uint8_t * , uint16_t);   
  int8_t (* Receive)       (uint8_t *, uint32_t *);  
} USBD_CDC_ItfTypeDef;

typedef struct {
  uint32_t data[CDC_DATA_FS_MAX_PACKET_SIZE/4];      /* Force 32bits alignment */
  uint8_t  CmdOpCode;
  uint8_t  CmdLength;    
  uint8_t  *RxBuffer;  
  uint8_t  *TxBuffer;   
  uint32_t RxLength;
  uint32_t TxLength;    
  
  __IO uint32_t TxState;     
  __IO uint32_t RxState;    
} USBD_CDC_HandleTypeDef;

typedef struct _USBD_STORAGE {
  int8_t (* Init) (uint8_t lun);
  int8_t (* GetCapacity) (uint8_t lun, uint32_t *block_num, uint16_t *block_size);
  int8_t (* IsReady) (uint8_t lun);
  int8_t (* IsWriteProtected) (uint8_t lun);
  int8_t (* Read) (uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
  int8_t (* Write)(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
  int8_t (* GetMaxLun)(void);
  int8_t *pInquiry;
} USBD_StorageTypeDef;

typedef struct {
  uint32_t                 max_lun;   
  uint32_t                 interface; 
  uint8_t                  bot_state;
  uint8_t                  bot_status;  
  uint16_t                 bot_data_length;
  uint8_t                  bot_data[MSC_MEDIA_PACKET];  
  USBD_MSC_BOT_CBWTypeDef  cbw;
  USBD_MSC_BOT_CSWTypeDef  csw;
  
  USBD_SCSI_SenseTypeDef   scsi_sense [SENSE_LIST_DEEPTH];
  uint8_t                  scsi_sense_head;
  uint8_t                  scsi_sense_tail;
  
  uint16_t                 scsi_blk_size;
  uint32_t                 scsi_blk_nbr;
  
  uint32_t                 scsi_blk_addr;
  uint32_t                 scsi_blk_len;
} USBD_MSC_BOT_HandleTypeDef;

extern USBD_ClassTypeDef USBD_CDC_MSC;

uint8_t  USBD_CDC_RegisterInterface  (USBD_HandleTypeDef   *pdev, USBD_CDC_ItfTypeDef *fops);
uint8_t  USBD_CDC_SetTxBuffer  (USBD_HandleTypeDef   *pdev, uint8_t  *pbuff, uint16_t length);
uint8_t  USBD_CDC_SetRxBuffer        (USBD_HandleTypeDef   *pdev, uint8_t  *pbuff);
uint8_t  USBD_CDC_ReceivePacket  (USBD_HandleTypeDef *pdev);
uint8_t  USBD_CDC_TransmitPacket  (USBD_HandleTypeDef *pdev);

uint8_t USBD_MSC_RegisterStorage(USBD_HandleTypeDef *pdev, USBD_StorageTypeDef *fops);

#endif // _USB_CDC_MSC_CORE_H_
