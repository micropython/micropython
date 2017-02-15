Example Description

This example describes how to use UART RX API with timeout.

Required Components:
    USBtoTTL adapter

Connect to PC
 - Connect Ground: connect to GND pin via USBtoTTL adapter
 - Use UART1
	GPIOA_6 as UART1_RX connect to TX of USBtoTTL adapter
	GPIOA_7 as UART1_TX connect to RX of USBtoTTL adapter

Open Super terminal or teraterm and 
set baud rate to 38400, 1 stopbit, no parity, no flow contorl.

This example shows:
1. The RX data ready interrupt service routine is used to receive characters from the PC, and then loopback them to the PC.
2. The TX done interrupt service routine will send the received string to the PC.
