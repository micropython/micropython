#ifndef R_SCI_UART_CFG_H_
#define R_SCI_UART_CFG_H_
#ifdef __cplusplus
extern "C" {
#endif

#define SCI_UART_CFG_PARAM_CHECKING_ENABLE (BSP_CFG_PARAM_CHECKING_ENABLE)
#define SCI_UART_CFG_FIFO_SUPPORT (0)
#define SCI_UART_CFG_DTC_SUPPORTED (0)
#define SCI_UART_CFG_FLOW_CONTROL_SUPPORT (0)
#define SCI_UART_CFG_RS485_SUPPORT (0)

/*
 * Disable r_sci_uart.c's interrupt handlers by defining following
 * because the renesas-ra has own interrupt handlers for SCI and
 * symbol conflict happens.
 */

#define SCI_UART_CFG_TX_ENABLE (0)
#define SCI_UART_CFG_RX_ENABLE (0)

#ifdef __cplusplus
}
#endif
#endif /* R_SCI_UART_CFG_H_ */
