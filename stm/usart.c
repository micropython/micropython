#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_usart.h>

#include "misc.h"
#include "usart.h"

static MP_BOOL is_enabled;

// USART6 on PC6 (TX), PC7 (RX)
void usart_init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART6, &USART_InitStructure);

    USART_Cmd(USART6, ENABLE);

    is_enabled = MP_TRUE;
}

MP_BOOL usart_is_enabled(void) {
    return is_enabled;
}

MP_BOOL usart_rx_any(void) {
    return USART_GetFlagStatus(USART6, USART_FLAG_RXNE) == SET;
}

int usart_rx_char(void) {
    return USART_ReceiveData(USART6);
}

void usart_tx_char(int c) {
    if (is_enabled) {
        // wait until the end of any previous transmission
        uint32_t timeout = 100000;
        while (USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET && --timeout > 0) {
        }
        USART_SendData(USART6, c);
    }
}

void usart_tx_str(const char *str) {
    for (; *str; str++) {
        usart_tx_char(*str);
    }
}

void usart_tx_strn_cooked(const char *str, int len) {
    for (const char *top = str + len; str < top; str++) {
        if (*str == '\n') {
            usart_tx_char('\r');
        }
        usart_tx_char(*str);
    }
}
