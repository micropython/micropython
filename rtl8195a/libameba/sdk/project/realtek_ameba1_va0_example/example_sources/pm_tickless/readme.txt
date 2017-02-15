Example Description

This example describes how to use freertos tickless with uart interruptable interface

Requirement Components:
	USBtoTTL adapter

Connect to PC
 - Connect Ground: connect to GND pin via USBtoTTL adapter
 - Use UART1
	GPIOA_0 as UART1_RX connect to TX of USBtoTTL adapter
	GPIOA_4 as UART1_TX connect to RX of USBtoTTL adapter

We also need connect GPIOC_1 as gpio interrupt which parallel with log uart rx pin.

In this example, freertos will enter/leave tickless automatically.
User can type continuous "Enter" in uart or log uart to wake system if system is in tickless.
System is keep awake until user type a command via uart.

There are some features in this example:
(1) We replace tickless' sleep function with system sleep api which save more power.
(2) Freertos enter tickless if the wakelock bit map is 0.
    It means there is no function require system keep awake.
    By default there is WAKELOCK_OS keep system awake.
    So we need release this WAKELOCK_OS enable tickless mode.
(3) We configure uart rx as gpio interrupt mode. This make uart can wake system.

NOTICE: If you don't want loss any data from treating UART signal as GPIO interrupt,
        you can set FREERTOS_PMU_TICKLESS_PLL_RESERVED to 1 in "platform_opt.h".
        It will reserved PLL clock in tickless and UART can receive the whole data.
        But it also cost more power consumption.
