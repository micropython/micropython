#ifndef _USB_CDC_MSC_CORE_H_
#define _USB_CDC_MSC_CORE_H_

#include  "usbd_cdc_msc_hid0.h"
#include  "usbd_msc_bot.h"
#include  "usbd_msc_scsi.h"
#include  "usbd_ioreq.h"

// CDC, MSC and HID packet sizes
#define CDC_DATA_FS_MAX_PACKET_SIZE (64) // endpoint IN & OUT packet size
#define MSC_MEDIA_PACKET            (2048) // was 8192; how low can it go whilst still working?
#define HID_DATA_FS_MAX_PACKET_SIZE (64) // endpoint IN & OUT packet size

// Need to define here for BOT and SCSI layers
#define MSC_IN_EP     (0x81)
#define MSC_OUT_EP    (0x01)

// Need to define here for usbd_cdc_interface.c (it needs CDC_IN_EP)
#define CDC_IN_EP     (0x83)
#define CDC_OUT_EP    (0x03)
#define CDC_CMD_EP    (0x82)

typedef struct {
  uint32_t bitrate;
  uint8_t  format;
  uint8_t  paritytype;
  uint8_t  datatype;
} USBD_CDC_LineCodingTypeDef;

typedef struct {
  uint32_t data[CDC_DATA_FS_MAX_PACKET_SIZE/4];      /* Force 32bits alignment */
  uint8_t  CmdOpCode;
  uint8_t  CmdLength;    
  
  __IO uint32_t TxState;     
  __IO uint32_t RxState;    
} USBD_CDC_HandleTypeDef;

typedef struct _USBD_STORAGE {
  int8_t (* Init) (uint8_t lun);
  int8_t (* GetCapacity) (uint8_t lun, uint32_t *block_num, uint16_t *block_size);
  int8_t (* IsReady) (uint8_t lun);
  int8_t (* IsWriteProtected) (uint8_t lun);
  int8_t (* StartStopUnit)(uint8_t lun, uint8_t started);
  int8_t (* PreventAllowMediumRemoval)(uint8_t lun, uint8_t param0);
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
  
  uint32_t                 scsi_blk_addr_in_blks;
  uint32_t                 scsi_blk_len;
} USBD_MSC_BOT_HandleTypeDef;

typedef struct _usbd_cdc_msc_hid_state_t {
    void *cdc;
    void *msc;
    void *hid;
} usbd_cdc_msc_hid_state_t;

#define USBD_HID_MOUSE_MAX_PACKET          (4)
#define USBD_HID_MOUSE_REPORT_DESC_SIZE    (74)

extern const uint8_t USBD_HID_MOUSE_ReportDesc[USBD_HID_MOUSE_REPORT_DESC_SIZE];

#define USBD_HID_KEYBOARD_MAX_PACKET       (8)
#define USBD_HID_KEYBOARD_REPORT_DESC_SIZE (63)

extern const uint8_t USBD_HID_KEYBOARD_ReportDesc[USBD_HID_KEYBOARD_REPORT_DESC_SIZE];

extern const USBD_ClassTypeDef USBD_CDC_MSC_HID;

// returns 0 on success, -1 on failure
int USBD_SelectMode(uint32_t mode, USBD_HID_ModeInfoTypeDef *hid_info);
// returns the current usb mode
uint8_t USBD_GetMode();

uint8_t USBD_CDC_ReceivePacket(USBD_HandleTypeDef *pdev, uint8_t *buf);
uint8_t USBD_CDC_TransmitPacket(USBD_HandleTypeDef *pdev, size_t len, const uint8_t *buf);

uint8_t USBD_MSC_RegisterStorage(USBD_HandleTypeDef *pdev, USBD_StorageTypeDef *fops);

uint8_t USBD_HID_ReceivePacket(USBD_HandleTypeDef *pdev, uint8_t *buf);
int USBD_HID_CanSendReport(USBD_HandleTypeDef *pdev);
uint8_t USBD_HID_SendReport(USBD_HandleTypeDef *pdev, uint8_t *report, uint16_t len);
uint8_t USBD_HID_SetNAK(USBD_HandleTypeDef *pdev);
uint8_t USBD_HID_ClearNAK(USBD_HandleTypeDef *pdev);

// These are provided externally to implement the CDC interface
struct _usbd_cdc_itf_t;
uint8_t *usbd_cdc_init(struct _usbd_cdc_itf_t *cdc, USBD_HandleTypeDef *pdev);
int8_t usbd_cdc_control(struct _usbd_cdc_itf_t *cdc, uint8_t cmd, uint8_t* pbuf, uint16_t length);
int8_t usbd_cdc_receive(struct _usbd_cdc_itf_t *cdc, size_t len);

// These are provided externally to implement the HID interface
struct _usbd_hid_itf_t;
uint8_t *usbd_hid_init(struct _usbd_hid_itf_t *hid, USBD_HandleTypeDef *pdev);
int8_t usbd_hid_receive(struct _usbd_hid_itf_t *hid, size_t len);

#endif // _USB_CDC_MSC_CORE_H_
