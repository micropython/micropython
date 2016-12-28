/*
 * $ Copyright Broadcom Corporation $
 */

/** @file
 *
 * Proximity Monitor Sample Application
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "pinmap_hal.h"

/* Exported defines ----------------------------------------------------------*/

#define TOTAL_HCI_USARTS              1
#define HCI_USART_BUFFER_SIZE         512

/* Exported types ------------------------------------------------------------*/
typedef struct HCI_USART_Ring_Buffer
{
    unsigned char buffer[HCI_USART_BUFFER_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;
} HCI_USART_Ring_Buffer;

typedef enum HAL_HCI_USART_Serial {
    HAL_HCI_USART_SERIAL6 = 0    //maps to BT_UART
} HAL_HCI_USART_Serial;

typedef void (*ReceiveHandler_t)(void);

/* Function Prototypes -------------------------------------------------------*/
void    HAL_HCI_USART_registerReceiveHandler(ReceiveHandler_t handler);
void    HAL_HCI_USART_receiveEvent(void);
int32_t HAL_HCI_USART_downloadFirmeare(HAL_HCI_USART_Serial serial);

void    HAL_HCI_USART_Init(HAL_HCI_USART_Serial serial, HCI_USART_Ring_Buffer *rx_buffer, HCI_USART_Ring_Buffer *tx_buffer);
void    HAL_HCI_USART_Begin(HAL_HCI_USART_Serial serial, uint32_t baud);
void    HAL_HCI_USART_End(HAL_HCI_USART_Serial serial);
void    HAL_HCI_USART_RestartSend(HAL_HCI_USART_Serial serial);

int32_t HAL_HCI_USART_Write_Data(HAL_HCI_USART_Serial serial, uint8_t data);
int32_t HAL_HCI_USART_Write_Buffer(HAL_HCI_USART_Serial serial, const uint8_t *buf, uint16_t len);
int32_t HAL_HCI_USART_Available_Data(HAL_HCI_USART_Serial serial);
int32_t HAL_HCI_USART_Read_Data(HAL_HCI_USART_Serial serial);

#ifdef __cplusplus
}
#endif
