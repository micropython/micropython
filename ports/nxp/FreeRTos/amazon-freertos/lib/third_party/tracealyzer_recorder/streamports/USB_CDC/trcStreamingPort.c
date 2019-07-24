#include "trcRecorder.h"

#if (TRC_USE_TRACEALYZER_RECORDER == 1)
#if(TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#include "stdint.h"

/* Include files as needed, in this case it is files from STM32Cube FW_F7 V1.4.1 */
#include "usb_device.h"
#include "usbd_cdc.h"
#include "usbd_CDC_if.h"
#include "usb_device.h"

#define BUFSIZE 64
	
typedef struct{
	uint32_t idx;
	uint8_t data[BUFSIZE];
}recBuf;

/* Define size for the receive and transmit buffer over CDC */
#define APP_RX_DATA_SIZE  8
#define APP_TX_DATA_SIZE  64

/* Received Data over USB are stored in this buffer       */
uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];

/* Send Data over USB CDC are stored in this buffer       */
uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

extern USBD_HandleTypeDef hUsbDeviceFS;
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;


recBuf commandBuffer;

static int8_t CDC_Init_FS     (void);
static int8_t CDC_DeInit_FS   (void);
static int8_t CDC_Control_FS  (uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Receive_FS  (uint8_t* pbuf, uint32_t *Len);
  
USBD_CDC_ItfTypeDef USBD_Interface_fops_FS = 
{
  CDC_Init_FS,
  CDC_DeInit_FS,
  CDC_Control_FS,  
  CDC_Receive_FS
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  CDC_Init_FS
  *         Initializes the CDC media low layer over the FS USB IP
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Init_FS(void)
{ 
  /* Set Application Buffers */
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, 0);
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);
  return (USBD_OK);
}

/**
  * @brief  CDC_DeInit_FS
  *         DeInitializes the CDC media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_DeInit_FS(void)
{ 
  return (USBD_OK);
}

/**
  * @brief  CDC_Control_FS
  *         Manage the CDC class requests
  * @param  cmd: Command code            
  * @param  pbuf: Buffer containing command data (request parameters)
  * @param  length: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Control_FS  (uint8_t cmd, uint8_t* pbuf, uint16_t length)
{ 
	switch (cmd)
  {
  case CDC_SEND_ENCAPSULATED_COMMAND:
		break;

  case CDC_GET_ENCAPSULATED_RESPONSE:
		break;

  case CDC_SET_COMM_FEATURE:
		break;

  case CDC_GET_COMM_FEATURE:
		break;

  case CDC_CLEAR_COMM_FEATURE:
		break;

  /*******************************************************************************/
  /* Line Coding Structure                                                       */
  /*-----------------------------------------------------------------------------*/
  /* Offset | Field       | Size | Value  | Description                          */
  /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
  /* 4      | bCharFormat |   1  | Number | Stop bits                            */
  /*                                        0 - 1 Stop bit                       */
  /*                                        1 - 1.5 Stop bits                    */
  /*                                        2 - 2 Stop bits                      */
  /* 5      | bParityType |  1   | Number | Parity                               */
  /*                                        0 - None                             */
  /*                                        1 - Odd                              */ 
  /*                                        2 - Even                             */
  /*                                        3 - Mark                             */
  /*                                        4 - Space                            */
  /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
  /*******************************************************************************/
  case CDC_SET_LINE_CODING:
		break;

  case CDC_GET_LINE_CODING:
		break;

  case CDC_SET_CONTROL_LINE_STATE:
		break;

  case CDC_SEND_BREAK:
		break;    
    
  default:
    break;
  }
  return (USBD_OK);
}

/**
  * @brief  CDC_Receive_FS
  *         Data received over USB OUT endpoint are sent over CDC interface 
  *         through this function.
  *           
  *         @note
  *         This function will block any OUT packet reception on USB endpoint 
  *         until exiting this function. If you exit this function before transfer
  *         is complete on CDC interface (i.e. using DMA controller) it will result 
  *         in receiving more data while previous ones are still not sent.
  *                 
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Receive_FS (uint8_t* Buf, uint32_t *Len)
{
	for( uint32_t i=0;i<* Len;i++)
	{		
		commandBuffer.data[commandBuffer.idx]=Buf[i];
		commandBuffer.idx++;
	}	
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);	

  return (USBD_OK);
}

/**
  * @brief  CDC_Transmit_FS
  *         Data send over USB IN endpoint are sent over CDC interface 
  *         through this function.           
  *         @note
  *         
  *                 
  * @param  Buf: Buffer of data to be send
  * @param  Len: Number of data to be send (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
  uint8_t result = USBD_OK;
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;
  if (hcdc->TxState != 0){
    return USBD_BUSY;
  }
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
  result = USBD_CDC_TransmitPacket(&hUsbDeviceFS);
  return result;
}

int32_t trcCDCReceive(void *data, uint32_t size, int32_t* NumBytes)
{
	uint32_t i,diff;

	if(commandBuffer.idx>0)
	{
		if (size >= commandBuffer.idx) // more than what is stored, number of bytes will be .idx
		{
			memcpy(data,commandBuffer.data, commandBuffer.idx);
			*NumBytes=commandBuffer.idx;
			commandBuffer.idx=0; // Make the buffer ready for a new command
		}
		else  //If some data in the buffer is not read
		{
			diff = commandBuffer.idx-size;
			memcpy(data,commandBuffer.data, size);
			for(i=0;i<diff;i++)
			{
				commandBuffer.data[i]=commandBuffer.data[i+size];
			}
			*NumBytes=size;
			commandBuffer.idx=diff;
		}
	}
	else
	{
		*NumBytes=0;
	}
	return 0;
}

int32_t trcCDCTransmit(void* data, uint32_t size, int32_t * noOfBytesSent )
{
	int32_t result;
	result=CDC_Transmit_FS(data,size);
	*noOfBytesSent=size;

	return result;
}

/**
* @brief This function handles USB On The Go FS global interrupt.
*/
void OTG_FS_IRQHandler(void)
{
  HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}

#endif	/*(TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)*/
#endif  /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/

