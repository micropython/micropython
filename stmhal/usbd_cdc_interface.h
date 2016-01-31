/*
 * This file is part of the Micro Python project, http://micropython.org/
 */

/**
  ******************************************************************************
  * @file    USB_Device/CDC_Standalone/Inc/usbd_cdc_interface.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    26-February-2014
  * @brief   Header for usbd_cdc_interface.c file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

extern const USBD_CDC_ItfTypeDef USBD_CDC_fops;

int USBD_CDC_IsConnected(void);
void USBD_CDC_SetInterrupt(int chr, void *data);

int USBD_CDC_TxHalfEmpty(void);
int USBD_CDC_Tx(const uint8_t *buf, uint32_t len, uint32_t timeout);
void USBD_CDC_TxAlways(const uint8_t *buf, uint32_t len);

int USBD_CDC_RxNum(void);
int USBD_CDC_Rx(uint8_t *buf, uint32_t len, uint32_t timeout);
