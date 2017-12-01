/**
 * \file uart.h
 * \author Stefan Naumann
 * \date 17. May 2017
 * \brief Code for the AUX-miniUART of the Raspberry Pi
 **/

#ifndef PIOS_UART
#define PIOS_UART

#include "gpio.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * \brief checks the current function of the UART-GPIO-Pins and returns whether they are correctly set or not
 **/
bool pios_uart_checkPins ();
/**
 * \brief initiate the miniUART of the AUX-peripheral
 **/
void pios_uart_init ( );
/**
 * \brief write a string to the UART
 * \param[in] str the string to be transmitted
 * \param[in] len the length of the string to be sent
 **/
void pios_uart_write ( const char* str, size_t len );
/**
 * \brief read a string of data from the UART-queue
 * \param[out] buff the buffer to be written to
 * \param[in] len the maximal length of data to be read
 **/
void pios_uart_read ( char* buff, size_t len );

/**
 * \brief write a single character to the transmitter-queue
 * \param[in] c the character to print
 **/
void pios_uart_putchar ( const char c );
/**
 * \brief read a single character from the UART
 * \return the single character
 **/
uint32_t pios_uart_getchar ( );
/**
 * \brief set a specific baudrate to the UART
 * \param[in] baud the wanted baudrate
 **/
void pios_uart_setBaud ( uint32_t baud );
/**
 * \brief set the data-size of the uart (7 or 8 bit)
 * \param[in] size the data-size of the characters in bits
 **/
void pios_uart_setDataSize ( int size );
/**
 * \brief print a string to the uart
 * \param[in] str the string to be transmitted
 **/
void pios_uart_puts ( const char* str );

/**
 * \brief is the receiver-queue ready?
 **/
bool pios_uart_rxReady ();
/**
 * \brief is the transmitter-queue ready?
 **/
bool pios_uart_txReady ();

/**
 * \brief get the receiver-queue-state
 **/
int pios_uart_rxQueue ();
/**
 * \brief get the transmitter-queue-state
 **/
int pios_uart_txQueue ();

/**
 * \brief wait until the transmitter is idle again, i.e. sent it's complete queue
 **/
void pios_uart_flush ();

#endif
