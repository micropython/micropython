extern USBD_Class_cb_TypeDef USBD_PYB_cb;
extern USBD_Class_cb_TypeDef USBD_PYB_HID_cb;

uint8_t USBD_HID_SendReport(USB_OTG_CORE_HANDLE *pdev, uint8_t *report, uint16_t len);
