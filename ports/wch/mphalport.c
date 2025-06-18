#include "debug.h"

void DeviceInit(void) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_Init();
    USART_Printf_Init(115200);
}

void mp_hal_stdout_tx_strn(const char *str, uint16_t len) {
    int i = 0;

    while (i < len) {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) {
            ;
        }
        USART_SendData(USART1, str[i++]);
    }
}

uint16_t mp_hal_stdin_rx_chr(void) {
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) {
        ;
    }
    return USART_ReceiveData(USART1);
}
