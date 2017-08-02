#include "uart.h"

#ifdef PIOS_PLATFORM_RPI

/// will be used as offset with the PBASE-address to get the AUX address 
/// in the I/O-device memory section
#define AUX_BASE_ADDR 0x00215000

/// system clock for the baudrate calculation
#define PIOS_AUX_SYSCLOCK 250000000

/**
 * \brief enum type for enumerating the registers of the AUX-peripheral of BCM2835
 * \note see section 2.1.1 in the manual
 **/
enum pios_aux_t
{
    AUX_IRQ         =  0,       ///< AUX Interrupt status (for hierarchical source-detection)
    AUX_ENABLE      =  1,       ///< AUX enable (last 3 bits: SPI2, SPI1, UART [2,1,0])
    
    AUX_MU_IO       = 16,       ///< UART I/O Data
    AUX_MU_IER      = 17,       ///< UART IRQ Enable
    AUX_MU_IIR      = 18,       ///< UART IRQ Identify
    AUX_MU_LCR      = 19,       ///< UART Line Control
    AUX_MU_MCR      = 20,       ///< UART Modem Control
    AUX_MU_LSR      = 21,       ///< UART Line Status
    AUX_MU_MSR      = 22,       ///< UART Modem Status
    AUX_MU_SCRATCH  = 23,       ///< UART Scratch ( an extra 8 bit buffer(?) )
    AUX_MU_CNTL     = 24,       ///< UART Extra Control
    AUX_MU_STAT     = 25,       ///< UART Extra Status
    AUX_MU_BAUD     = 26,       ///< UART Baudrate
    
    AUX_SPI0_CNTL0  = 32,       ///< SPI 0 Control Register 0
    AUX_SPI0_CNTL1  = 33,       ///< SPI 0 Control Register 1
    AUX_SPI0_STAT   = 34,       ///< SPI 0 Status
    AUX_SPI0_IO     = 35,       ///< SPI 0 Data
    AUX_SPI0_PEEK   = 36,       ///< SPI 0 Peek
    
    AUX_SPI1_CNTL0  = 48,       ///< SPI 1 Control Register 0
    AUX_SPI1_CNTL1  = 49,       ///< SPI 1 Control Register 1
    AUX_SPI1_STAT   = 50,       ///< SPI 1 Status
    AUX_SPI1_IO     = 51,       ///< SPI 1 Data
    AUX_SPI1_PEEK   = 52        ///< SPI 1 Peek
};

/// enable bit for the ENABLE-operation
#define AUX_UART ( 1 << 0 )
/// bit for SPI0 ENABLE-operation
#define AUX_SPI0 ( 1 << 1 )
/// bit for SPI1 ENABLE-operation
#define AUX_SPI1 ( 1 << 2 )

/// bit for finding out whether the Transmitter is idle
#define AUX_TX_IDLE ( 1 << 6 )
/// can the transmitter-queue receive another byte of data
#define AUX_TX_EMPTY ( 1 << 5 )
/// is the receiver overrung, i.e. were data lost
#define AUX_RX_OVERRUN ( 1 << 1 )
/// is there a byte in the receiver-queue?
#define AUX_RX_DATA ( 1 << 0 )

/**
 * \brief mapping of AUX-registers, use with pios_aux_t as indices
 **/
extern volatile uint32_t* const pios_aux;

volatile uint32_t* const pios_aux = (volatile uint32_t* const) (PIOS_IO_BASE + AUX_BASE_ADDR);

void pios_uart_init ( )
{    
    // set the GPIO-pins into the correct state   
    pios_aux[AUX_ENABLE] = AUX_UART;
    pios_aux[AUX_MU_IER] = 0;
    pios_aux[AUX_MU_CNTL] = 0;
    pios_aux[AUX_MU_LCR] = 3;       // undocumented second bit (?)
    pios_aux[AUX_MU_MCR] = 0;
    pios_aux[AUX_MU_IER] = 0;
    pios_aux[AUX_MU_IIR] = 0xc6;
    pios_aux[AUX_MU_BAUD] = 270;    // sets the BAUDrate to 115200
                
                
    pios_gpio_pinmode ( 14, PIOS_GPIO_ALT5 );
    pios_gpio_pinmode ( 15, PIOS_GPIO_ALT5 );
    /**
     * set-up of Pull Up / Down: 
       *    1. Write to GPPUD to set the required control signal (i.e. Pull-up or Pull-Down or neither to remove the current Pull-up/down)
       *    2. Wait 150 cycles – this provides the required set-up time for the control signal
       *    3. Write to GPPUDCLK0/1 to clock the control signal into the GPIO pads you wish to modify – NOTE only the pads which receive a clock will be modified, all others will retain their previous state.
       *    4. Wait 150 cycles – this provides the required hold time for the control signal
       *    5. Write to GPPUD to remove the control signal
       *    6. Write to GPPUDCLK0/1 to remove the clock
       * see: Manual pg 101
    **/

    uint32_t p[2] = { (1<<14)|(1<<15), 0 };
    pios_gpio_pullBulk ( p, PIOS_GPIO_PULL_OFF );
    
    pios_aux[AUX_MU_CNTL] = 3;
}

bool pios_uart_checkPins ()
{
    if ( pios_gpio_getPinmode ( 14 ) == PIOS_GPIO_ALT5  && 
         pios_gpio_getPinmode ( 15 ) == PIOS_GPIO_ALT5 )
    {
        return true;
    }
    return false;
}

void pios_uart_puts ( const char* str )
{
    while ( *str )
    {
        pios_uart_putchar(*str);
        str++;
    }
}

void pios_uart_write ( const char* str, size_t len )
{
    for ( int i=0; i<len; i++ )
    {
        pios_uart_putchar ( str[i] );
    }
}
void pios_uart_read ( char* buff, size_t len )
{
    for ( int i=0; i<len; i++ )
    {
        buff[i] = (char) pios_uart_getchar();
    }
}

void pios_uart_putchar ( const char c )
{
    while ( 1 ) 
    {
        if ((pios_aux[AUX_MU_LSR] & AUX_TX_EMPTY) != 0)
            break;
    }
    pios_aux[AUX_MU_IO] = ((0xff) & c);
}

uint32_t pios_uart_getchar ( )
{
    while ( 1 ) 
    {
        if ((pios_aux[AUX_MU_LSR] & AUX_RX_DATA) != 0 ) 
            break;
    }
    return (0xff & pios_aux[AUX_MU_IO]);
}

int pios_uart_getchar_asynch ( uint8_t* data )
{
    if ((pios_aux[AUX_MU_LSR] & AUX_RX_DATA) != 0 ) 
    {
        *data = (0xff & pios_aux[AUX_MU_IO]);
        return 0;
    }
    return -1;
}   

void pios_uart_setBaud ( uint32_t baud )
{
    pios_aux[AUX_MU_BAUD] = (PIOS_AUX_SYSCLOCK / (baud<<3)) + 1;
}

void pios_uart_setDataSize ( int size )
{
    uint32_t val = pios_aux[AUX_MU_LCR];
    // öhm öhm öhm
    val = (val & 0xfffffffe) | ((size == 8) ? 1 : 0);   // 8 bit mode
    pios_aux[AUX_MU_LCR] = val;
}

bool pios_uart_rxReady ()
{
    return ((pios_aux[AUX_MU_LSR] & AUX_RX_DATA) == 0 );
}

bool pios_uart_txReady ()
{
    return ((pios_aux[AUX_MU_LSR] & AUX_TX_EMPTY) == 0 );
}

int pios_uart_rxQueue ()
{
    return (((0x0f000000) & pios_aux[AUX_MU_STAT]) >> 24);
}
int pios_uart_txQueue ()
{
    return (((0x000f0000) & pios_aux[AUX_MU_STAT]) >> 16);
}

#endif

