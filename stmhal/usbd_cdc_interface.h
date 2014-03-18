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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CDC_IF_H
#define __USBD_CDC_IF_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Definition for TIMx clock resources */
#define USBD_CDC_TIMx                             TIM3
#define USBD_CDC_TIMx_CLK_ENABLE                  __TIM3_CLK_ENABLE
#define USBD_CDC_TIMx_FORCE_RESET()               __USART3_FORCE_RESET()
#define USBD_CDC_TIMx_RELEASE_RESET()             __USART3_RELEASE_RESET()

/* Definition for TIMx's NVIC */
#define USBD_CDC_TIMx_IRQn                        TIM3_IRQn
//#define USBD_CDC_TIMx_IRQHandler                  TIM3_IRQHandler // this is hard coded in stm32f4xx_it.c

/* Periodically, the state of the buffer "UserTxBuffer" is checked.
   The period depends on USBD_CDC_POLLING_INTERVAL */
#define USBD_CDC_POLLING_INTERVAL             10 /* in ms. The max is 65 and the min is 1 */

extern const USBD_CDC_ItfTypeDef USBD_CDC_fops;

void USBD_CDC_SetInterrupt(int chr, void *data);
void USBD_CDC_Tx(const char *str, uint32_t len);
int USBD_CDC_RxNum(void);
int USBD_CDC_RxGet(void);

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
#endif /* __USBD_CDC_IF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
