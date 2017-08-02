/**
 * \file qemu/uart.c
 * \brief code for the qemu virtual UART (which apparently emulates a PrimeCell UART)
 * see http://infocenter.arm.com/help/topic/com.arm.doc.ddi0183f/DDI0183.pdf for reference
 **/

#include "uart.h"

#ifdef PIOS_PLATFORM_QEMU

#define PIOS_QEMU_UART_BASE 0x101f1000

enum _pios_qemu_uart_t
{
    QEMU_UART_DR     = 0x0000,      ///< data register
    QEMU_UART_RSR    = 0x0001,      ///< receive status register / error clear registe
    
    QEMU_UART_FR     = 0x0006,      ///< flag register
    QEMU_UART_ILPR   = 0x0008,      ///< low-power counter register
    QEMU_UART_IBRD   = 0x0009,      ///< integer baud rate register
    QEMU_UART_FBRD   = 0x000a,      ///< fractional baud rate register
    QEMU_UART_LCR_H  = 0x000b,      ///< line control register
    QEMU_UART_CR     = 0x000c,      ///< control register
    QEMU_UART_IFLS   = 0x000d,      ///< interrupt fifo level select register
    QEMU_UART_IMSC   = 0x000e,      ///< interrupt mask set/clear register
    QEMU_UART_RIS    = 0x000f,      ///< raw interrupt status register
    QEMU_UART_MIS    = 0x0010,      ///< masked interrupt status regsiter
    QEMU_UART_ICR    = 0x0011,      ///< interrupt clear register
    QEMU_UART_MACR   = 0x0012,      ///< DMA control register
    
    QEMU_UART_PID0   = 0x03f8,      ///< don't ask me
    QEMU_UART_PID1   = 0x03f9,      ///< nope, no idea
    QEMU_UART_PID2   = 0x03fa,      ///< really, no idea what this does
    QEMU_UART_PID3   = 0x03fb,      ///< well actually...
    QEMU_UART_PCELL0 = 0x03fc,      ///< I don't have a clue
    QEMU_UART_PCELL1 = 0x03fd,      ///< see the manual and find out yourself
    QEMU_UART_PCELL2 = 0x03fe,      ///< because I don't know
    QEMU_UART_PCELL3 = 0x03ff,      ///< nor do I care enough
} pios_qemu_uart_t;

volatile uint32_t* const pios_qemu_uart = (volatile uint32_t* const) PIOS_QEMU_UART_BASE;

void pios_uart_init ( )
{
    
}

void pios_uart_write ( const char* str, size_t len )
{
    for ( ; len>0; len-- )
    {
        pios_uart_putchar ( *str );
        str++;
    }
}

void pios_uart_read ( char* buff, size_t len )
{
    for ( size_t i = 0; i<len; i++ )
    {
        buff[i] = pios_uart_getchar ();
    }
}


void pios_uart_putchar ( const char c )
{
    while (1)
    {
        if ( (pios_qemu_uart[QEMU_UART_FR] & (1 << 5)) == 0 )     /// transmit FIFO full?
            break;
    }
    pios_qemu_uart[QEMU_UART_DR] = c;
}

uint32_t pios_uart_getchar ( )
{
    while ( (pios_qemu_uart[QEMU_UART_FR] & (1 << 4)) != 0)
    {

    }
    return pios_qemu_uart[QEMU_UART_DR];
}

void pios_uart_puts ( const char* str )
{
    pios_uart_write ( str, strlen( str ) );
}
void pios_uart_flush ()
{
    while (1)
    {
        if ( (pios_qemu_uart[QEMU_UART_FR] & (1 << 5)) == 0 )     /// transmit FIFO full?
            break;
    }
}

bool pios_uart_rxReady () {}
bool pios_uart_txReady () {}
int pios_uart_rxQueue () {}
int pios_uart_txQueue () {}
void pios_uart_setBaud ( uint32_t baud ) {}
void pios_uart_setDataSize ( int size ) {}

#endif
