/**
  ******************************************************************************
  * @file    USB_Device/CDC_Standalone/Src/usbd_cdc_interface.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    26-February-2014
  * @brief   Source file for USBD CDC interface
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

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "usbd_cdc.h"
#include "usbd_cdc_interface.h"
#include "pendsv.h"
#include "usb.h"

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */

/** @defgroup USBD_CDC 
  * @brief usbd core module
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define APP_RX_DATA_SIZE  2048 // I think this must be at least CDC_DATA_FS_OUT_PACKET_SIZE
#define APP_TX_DATA_SIZE  2048 // I think this can be any value

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

uint8_t UserRxBuffer[APP_RX_DATA_SIZE];/* Received Data over USB are stored in this buffer */
uint32_t UserRxBufLen; // counts number of valid characters in UserRxBuffer

uint8_t UserTxBuffer[APP_TX_DATA_SIZE];/* Received Data over UART (CDC interface) are stored in this buffer */
uint32_t UserTxBufPtrIn = 0;/* Increment this pointer or roll it back to
                               start address when data are received over USART */
uint32_t UserTxBufPtrOut = 0; /* Increment this pointer or roll it back to
                                 start address when data are sent over USB */

static int user_interrupt_char = VCP_CHAR_NONE;
static void *user_interrupt_data = NULL;

#if 0
/* UART handler declaration */
UART_HandleTypeDef UartHandle;
#endif
/* TIM handler declaration */
TIM_HandleTypeDef USBD_CDC_TimHandle;
/* USB handler declaration */
extern USBD_HandleTypeDef hUSBDDevice;

/* Private function prototypes -----------------------------------------------*/
static int8_t CDC_Itf_Init     (void);
static int8_t CDC_Itf_DeInit   (void);
static int8_t CDC_Itf_Control  (uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Itf_Receive  (uint8_t* pbuf, uint32_t *Len);

static void Error_Handler(void);
//static void ComPort_Config(void);
static void TIM_Config(void);

USBD_CDC_ItfTypeDef USBD_CDC_fops = 
{
  CDC_Itf_Init,
  CDC_Itf_DeInit,
  CDC_Itf_Control,
  CDC_Itf_Receive
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  CDC_Itf_Init
  *         Initializes the CDC media low layer
  * @param  None
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Itf_Init(void)
{
#if 0
  /*##-1- Configure the UART peripheral ######################################*/
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* USART configured as follow:
      - Word Length = 8 Bits
      - Stop Bit    = One Stop bit
      - Parity      = No parity
      - BaudRate    = 115200 baud
      - Hardware flow control disabled (RTS and CTS signals) */
  UartHandle.Instance        = USARTx;
  UartHandle.Init.BaudRate   = 115200;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits   = UART_STOPBITS_1;
  UartHandle.Init.Parity     = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode       = UART_MODE_TX_RX;
  
  if(HAL_UART_Init(&UartHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  /*##-2- Put UART peripheral in IT reception process ########################*/
  /* Any data received will be stored in "UserTxBuffer" buffer  */
  if(HAL_UART_Receive_IT(&UartHandle, (uint8_t *)UserTxBuffer, 1) != HAL_OK)
  {
    /* Transfer error in reception process */
    Error_Handler();
  }
#endif
  
  /*##-3- Configure the TIM Base generation  #################################*/
  TIM_Config();
  
  /*##-4- Start the TIM Base generation in interrupt mode ####################*/
  /* Start Channel1 */
  if(HAL_TIM_Base_Start_IT(&USBD_CDC_TimHandle) != HAL_OK)
  {
    /* Starting Error */
    Error_Handler();
  }
  
  /*##-5- Set Application Buffers ############################################*/
  USBD_CDC_SetTxBuffer(&hUSBDDevice, UserTxBuffer, 0);
  USBD_CDC_SetRxBuffer(&hUSBDDevice, UserRxBuffer);
  UserRxBufLen = 0;
  
    user_interrupt_char = VCP_CHAR_NONE;
    user_interrupt_data = NULL;

  return (USBD_OK);
}

/**
  * @brief  CDC_Itf_DeInit
  *         DeInitializes the CDC media low layer
  * @param  None
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Itf_DeInit(void)
{
#if 0
  /* DeInitialize the UART peripheral */
  if(HAL_UART_DeInit(&UartHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
#endif
  return (USBD_OK);
}

/**
  * @brief  CDC_Itf_Control
  *         Manage the CDC class requests
  * @param  Cmd: Command code            
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Itf_Control (uint8_t cmd, uint8_t* pbuf, uint16_t length)
{ 
  switch (cmd)
  {
  case CDC_SEND_ENCAPSULATED_COMMAND:
    /* Add your code here */
    break;

  case CDC_GET_ENCAPSULATED_RESPONSE:
    /* Add your code here */
    break;

  case CDC_SET_COMM_FEATURE:
    /* Add your code here */
    break;

  case CDC_GET_COMM_FEATURE:
    /* Add your code here */
    break;

  case CDC_CLEAR_COMM_FEATURE:
    /* Add your code here */
    break;

  case CDC_SET_LINE_CODING:
    #if 0
    LineCoding.bitrate    = (uint32_t)(pbuf[0] | (pbuf[1] << 8) |\
                            (pbuf[2] << 16) | (pbuf[3] << 24));
    LineCoding.format     = pbuf[4];
    LineCoding.paritytype = pbuf[5];
    LineCoding.datatype   = pbuf[6];
    
    /* Set the new configuration */
    ComPort_Config();
    #endif
    break;

  case CDC_GET_LINE_CODING:
    #if 0
    pbuf[0] = (uint8_t)(LineCoding.bitrate);
    pbuf[1] = (uint8_t)(LineCoding.bitrate >> 8);
    pbuf[2] = (uint8_t)(LineCoding.bitrate >> 16);
    pbuf[3] = (uint8_t)(LineCoding.bitrate >> 24);
    pbuf[4] = LineCoding.format;
    pbuf[5] = LineCoding.paritytype;
    pbuf[6] = LineCoding.datatype;     
    #endif
    
    /* Add your code here */
    pbuf[0] = (uint8_t)(115200);
    pbuf[1] = (uint8_t)(115200 >> 8);
    pbuf[2] = (uint8_t)(115200 >> 16);
    pbuf[3] = (uint8_t)(115200 >> 24);
    pbuf[4] = 0; // stop bits (1)
    pbuf[5] = 0; // parity (none)
    pbuf[6] = 8; // number of bits (8)
    break;

  case CDC_SET_CONTROL_LINE_STATE:
    /* Add your code here */
    break;

  case CDC_SEND_BREAK:
     /* Add your code here */
    break;    
    
  default:
    break;
  }
  
  return (USBD_OK);
}

/**
  * @brief  TIM period elapsed callback
  * @param  htim: TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  uint32_t buffptr;
  uint32_t buffsize;
  
  if(UserTxBufPtrOut != UserTxBufPtrIn)
  {
    if(UserTxBufPtrOut > UserTxBufPtrIn) /* rollback */
    {
      buffsize = APP_RX_DATA_SIZE - UserTxBufPtrOut;
    }
    else 
    {
      buffsize = UserTxBufPtrIn - UserTxBufPtrOut;
    }
    
    buffptr = UserTxBufPtrOut;
    
    USBD_CDC_SetTxBuffer(&hUSBDDevice, (uint8_t*)&UserTxBuffer[buffptr], buffsize);
    
    if(USBD_CDC_TransmitPacket(&hUSBDDevice) == USBD_OK)
    {
      UserTxBufPtrOut += buffsize;
      if (UserTxBufPtrOut == APP_RX_DATA_SIZE)
      {
        UserTxBufPtrOut = 0;
      }
    }
  }
}

#if 0
/**
  * @brief  Rx Transfer completed callback
  * @param  huart: UART handle
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Increment Index for buffer writing */
  UserTxBufPtrIn++;
  
  /* To avoid buffer overflow */
  if(UserTxBufPtrIn == APP_RX_DATA_SIZE)
  {
    UserTxBufPtrIn = 0;
  }
  
  /* Start another reception: provide the buffer pointer with offset and the buffer size */
  HAL_UART_Receive_IT(huart, (uint8_t *)(UserTxBuffer + UserTxBufPtrIn), 1);
}
#endif

/**
  * @brief  CDC_Itf_DataRx
  *         Data received over USB OUT endpoint is processed here.
  * @param  Buf: Buffer of data received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  * @note   The buffer we are passed here is just UserRxBuffer, so we are
  *         free to modify it.
  */
static int8_t CDC_Itf_Receive(uint8_t* Buf, uint32_t *Len) {
#if 0
    // this sends the data over the UART using DMA
    HAL_UART_Transmit_DMA(&UartHandle, Buf, *Len);
#endif

    if (user_interrupt_char == VCP_CHAR_NONE) {
        // no special interrupt character
        UserRxBufLen = *Len;

    } else {
        // filter out sepcial interrupt character from the buffer
        bool char_found = false;
        uint8_t *dest = Buf;
        uint8_t *src = Buf;
        uint8_t *buf_top = Buf + *Len;
        for (; src < buf_top; src++) {
            if (*src == user_interrupt_char) {
                char_found = true;
            } else {
                if (char_found) {
                    *dest = *src;
                }
                dest++;
            }
        }

        // set length of remaining characters
        UserRxBufLen = dest - Buf;

        if (char_found) {
            // raise exception when interrupts are finished
            user_interrupt_char = VCP_CHAR_NONE;
            pendsv_nlr_jump(user_interrupt_data);
        }
    }

    if (UserRxBufLen == 0) {
        // initiate next USB packet transfer now that UserRxBuffer has been drained
        USBD_CDC_ReceivePacket(&hUSBDDevice);
    }

    return (USBD_OK);
}

void USBD_CDC_SetInterrupt(int chr, void *data) {
    user_interrupt_char = chr;
    user_interrupt_data = data;
}

void USBD_CDC_Tx(const char *str, uint32_t len) {
    for (int i = 0; i < len; i++) {
        UserTxBuffer[UserTxBufPtrIn] = str[i];
        UserTxBufPtrIn = (UserTxBufPtrIn + 1) & (APP_TX_DATA_SIZE - 1);
    }
}

int USBD_CDC_RxAny(void) {
    return UserRxBufLen;
}

int USBD_CDC_RxGet(void) {
    while (UserRxBufLen == 0) {
        __WFI();
    }
    int c = UserRxBuffer[--UserRxBufLen];
    if (UserRxBufLen == 0) {
        // initiate next USB packet transfer now that UserRxBuffer has been drained
        USBD_CDC_ReceivePacket(&hUSBDDevice);
    }
    return c;
}

#if 0
/**
  * @brief  Tx Transfer completed callback
  * @param  huart: UART handle
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Initiate next USB packet transfer once UART completes transfer (transmitting data over Tx line) */
  USBD_CDC_ReceivePacket(&hUSBDDevice);
}

/**
  * @brief  ComPort_Config
  *         Configure the COM Port with the parameters received from host.
  * @param  None.
  * @retval None.
  * @note   When a configuration is not supported, a default value is used.
  */
static void ComPort_Config(void)
{
  if(HAL_UART_DeInit(&UartHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  /* set the Stop bit */
  switch (LineCoding.format)
  {
  case 0:
    UartHandle.Init.StopBits = UART_STOPBITS_1;
    break;
  case 2:
    UartHandle.Init.StopBits = UART_STOPBITS_2;
    break;
  default :
    UartHandle.Init.StopBits = UART_STOPBITS_1;
    break;
  }
  
  /* set the parity bit*/
  switch (LineCoding.paritytype)
  {
  case 0:
    UartHandle.Init.Parity = UART_PARITY_NONE;
    break;
  case 1:
    UartHandle.Init.Parity = UART_PARITY_ODD;
    break;
  case 2:
    UartHandle.Init.Parity = UART_PARITY_EVEN;
    break;
  default :
    UartHandle.Init.Parity = UART_PARITY_NONE;
    break;
  }
  
  /*set the data type : only 8bits and 9bits is supported */
  switch (LineCoding.datatype)
  {
  case 0x07:
    /* With this configuration a parity (Even or Odd) must be set */
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    break;
  case 0x08:
    if(UartHandle.Init.Parity == UART_PARITY_NONE)
    {
      UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    }
    else 
    {
      UartHandle.Init.WordLength = UART_WORDLENGTH_9B;
    }
    
    break;
  default :
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    break;
  }
  
  UartHandle.Init.BaudRate = LineCoding.bitrate;
  UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode       = UART_MODE_TX_RX;
  
  if(HAL_UART_Init(&UartHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Start reception: provide the buffer pointer with offset and the buffer size */
  HAL_UART_Receive_IT(&UartHandle, (uint8_t *)(UserTxBuffer + UserTxBufPtrIn), 1);
}
#endif

/**
  * @brief  TIM_Config: Configure TIMx timer
  * @param  None.
  * @retval None.
  */
static void TIM_Config(void)
{  
  /* Set TIMx instance */
  USBD_CDC_TimHandle.Instance = USBD_CDC_TIMx;
  
  /* Initialize TIM3 peripheral as follow:
       + Period = 10000 - 1
       + Prescaler = ((SystemCoreClock/2)/10000) - 1
       + ClockDivision = 0
       + Counter direction = Up
  */
  USBD_CDC_TimHandle.Init.Period = (USBD_CDC_POLLING_INTERVAL*1000) - 1;
  USBD_CDC_TimHandle.Init.Prescaler = 84-1;
  USBD_CDC_TimHandle.Init.ClockDivision = 0;
  USBD_CDC_TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
  if(HAL_TIM_Base_Init(&USBD_CDC_TimHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
}

#if 0
/**
  * @brief  UART error callbacks
  * @param  UartHandle: UART handle
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
  /* Transfer error occured in reception and/or transmission process */
  Error_Handler();
}
#endif

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* Add your own code here */
}

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

