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
#include "usbd_cdc_msc_hid.h"
#include "usbd_cdc_interface.h"
#include "pendsv.h"
#include "usb.h"

// CDC control commands
#define CDC_SEND_ENCAPSULATED_COMMAND               0x00
#define CDC_GET_ENCAPSULATED_RESPONSE               0x01
#define CDC_SET_COMM_FEATURE                        0x02
#define CDC_GET_COMM_FEATURE                        0x03
#define CDC_CLEAR_COMM_FEATURE                      0x04
#define CDC_SET_LINE_CODING                         0x20
#define CDC_GET_LINE_CODING                         0x21
#define CDC_SET_CONTROL_LINE_STATE                  0x22
#define CDC_SEND_BREAK                              0x23

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define APP_RX_DATA_SIZE  1024 // I think this must be at least CDC_DATA_FS_OUT_PACKET_SIZE=64 (APP_RX_DATA_SIZE was 2048)
#define APP_TX_DATA_SIZE  1024 // I think this can be any value (was 2048)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static uint8_t dev_is_connected = 0; // indicates if we are connected

static uint8_t UserRxBuffer[APP_RX_DATA_SIZE]; // received data from USB OUT endpoint is stored in this buffer
static uint16_t UserRxBufCur = 0; // points to next available character in UserRxBuffer
static uint16_t UserRxBufLen = 0; // counts number of valid characters in UserRxBuffer

static uint8_t UserTxBuffer[APP_TX_DATA_SIZE]; // data for USB IN endpoind is stored in this buffer
static uint16_t UserTxBufPtrIn = 0; // increment this pointer modulo APP_TX_DATA_SIZE when new data is available
static uint16_t UserTxBufPtrOut = 0; // increment this pointer modulo APP_TX_DATA_SIZE when data is drained

static int user_interrupt_char = VCP_CHAR_NONE;
static void *user_interrupt_data = NULL;

/* USB handler declaration */
extern USBD_HandleTypeDef hUSBDDevice;

/* Private function prototypes -----------------------------------------------*/
static int8_t CDC_Itf_Init     (void);
static int8_t CDC_Itf_DeInit   (void);
static int8_t CDC_Itf_Control  (uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Itf_Receive  (uint8_t* pbuf, uint32_t *Len);

const USBD_CDC_ItfTypeDef USBD_CDC_fops = {
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
  
  /*##-3- Configure the TIM Base generation  #################################*/
  now done in HAL_MspInit
  TIM_Config();
#endif
  
    /*##-4- Start the TIM Base generation in interrupt mode ####################*/
    /* Start Channel1 */
    __HAL_TIM_ENABLE_IT(&TIM3_Handle, TIM_IT_UPDATE);
  
    /*##-5- Set Application Buffers ############################################*/
    USBD_CDC_SetTxBuffer(&hUSBDDevice, UserTxBuffer, 0);
    USBD_CDC_SetRxBuffer(&hUSBDDevice, UserRxBuffer);

    UserRxBufCur = 0;
    UserRxBufLen = 0;
  
    /* NOTE: we cannot reset these here, because USBD_CDC_SetInterrupt
     * may be called before this init function to set these values.
     * This can happen if the USB enumeration occurs after the call to
     * USBD_CDC_SetInterrupt.
    user_interrupt_char = VCP_CHAR_NONE;
    user_interrupt_data = NULL;
    */

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
static int8_t CDC_Itf_Control(uint8_t cmd, uint8_t* pbuf, uint16_t length) {
    switch (cmd) {
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
            dev_is_connected = length & 1; // wValue is passed in Len (bit of a hack)
            break;

        case CDC_SEND_BREAK:
            /* Add your code here */
            break;

        default:
            break;
    }

    return USBD_OK;
}

/**
  * @brief  TIM period elapsed callback
  * @param  htim: TIM handle
  * @retval None
  */
void USBD_CDC_HAL_TIM_PeriodElapsedCallback(void) {
  uint32_t buffptr;
  uint32_t buffsize;
  
  if(UserTxBufPtrOut != UserTxBufPtrIn)
  {
    if(UserTxBufPtrOut > UserTxBufPtrIn) /* rollback */
    {
      buffsize = APP_TX_DATA_SIZE - UserTxBufPtrOut;
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
      if (UserTxBufPtrOut == APP_TX_DATA_SIZE)
      {
        UserTxBufPtrOut = 0;
      }
    }
  }
}

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

    // TODO improve this function to implement a circular buffer

    // if we have processed all the characters, reset the buffer counters
    if (UserRxBufCur > 0 && UserRxBufCur >= UserRxBufLen) {
        memmove(UserRxBuffer, UserRxBuffer + UserRxBufLen, *Len);
        UserRxBufCur = 0;
        UserRxBufLen = 0;
    }

    uint32_t delta_len;

    if (user_interrupt_char == VCP_CHAR_NONE) {
        // no special interrupt character
        delta_len = *Len;

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

        if (char_found) {
            // raise exception when interrupts are finished
            user_interrupt_char = VCP_CHAR_NONE;
            pendsv_nlr_jump(user_interrupt_data);
        }

        // length of remaining characters
        delta_len = dest - Buf;
    }

    if (UserRxBufLen + delta_len + CDC_DATA_FS_MAX_PACKET_SIZE > APP_RX_DATA_SIZE) {
        // if we keep this data then the buffer can overflow on the next USB rx
        // so we don't increment the length, and throw this data away
    } else {
        // data fits, leaving room for another CDC_DATA_FS_OUT_PACKET_SIZE
        UserRxBufLen += delta_len;
    }

    // initiate next USB packet transfer, to append to existing data in buffer
    USBD_CDC_SetRxBuffer(&hUSBDDevice, UserRxBuffer + UserRxBufLen);
    USBD_CDC_ReceivePacket(&hUSBDDevice);

    return USBD_OK;
}

int USBD_CDC_IsConnected(void) {
    return dev_is_connected;
}

void USBD_CDC_SetInterrupt(int chr, void *data) {
    user_interrupt_char = chr;
    user_interrupt_data = data;
}

void USBD_CDC_Tx(const char *str, uint32_t len) {
    for (int i = 0; i < len; i++) {
        uint timeout = 200;
        while (((UserTxBufPtrIn + 1) & (APP_TX_DATA_SIZE - 1)) == UserTxBufPtrOut) {
            if (timeout-- == 0) {
                break;
            }
            HAL_Delay(1);
        }
        UserTxBuffer[UserTxBufPtrIn] = str[i];
        UserTxBufPtrIn = (UserTxBufPtrIn + 1) & (APP_TX_DATA_SIZE - 1);
    }
}

int USBD_CDC_RxNum(void) {
    return UserRxBufLen - UserRxBufCur;
}

int USBD_CDC_RxGet(void) {
    // wait for buffer to have at least 1 character in it
    while (USBD_CDC_RxNum() == 0) {
        __WFI();
    }

    // get next character
    int c = UserRxBuffer[UserRxBufCur++];

    return c;
}
