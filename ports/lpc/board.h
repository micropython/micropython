#ifndef _LPC_BOARD_H_
#define _LPC_BOARD_H_

#include <chip.h>

#define CONSOLE_UART LPC_USART2
#define BOARD_CIAA_EDU_NXP_4337

#ifdef __cplusplus
extern "C" {
#endif

void Board_SystemInit(void);
void Board_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* _LPC_BOARD_H_ */
