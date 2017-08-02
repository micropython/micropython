#include "gpio.h"

volatile pios_gpio_t* const pios_gpio = (volatile pios_gpio_t* const) (PIOS_IO_BASE + PIOS_GPIO_BASE);

void __attribute__((optimize("O0"))) wait ( int sec )
{    
    while ( sec > 0 )
    {
        int counter = 0x400000;
        while ( counter > 0 )
            counter--; 
        sec--;
    }
}

void __attribute__((optimize("O1"))) pios_wasteCycles ( uint32_t cycles )
{
    //cycles /= 2;
    cycles++;
    while ( cycles > 0 ) { cycles--; }
}

void pios_gpio_pullBulk ( uint32_t p[2], uint32_t pull )
{
    if ( pull == PIOS_GPIO_PULL_UP || 
         pull == PIOS_GPIO_PULL_DOWN || 
         pull == PIOS_GPIO_PULL_OFF)
    {
        pios_gpio->pullControlEnable = pull;
        pios_wasteCycles ( 150 );
        
        pios_gpio->pullControlClock[0] = p[0];
        pios_gpio->pullControlClock[1] = p[1];
        pios_wasteCycles ( 150 );
        
        //pios_gpio->pullControlEnable = PIOS_GPIO_PULL_OFF;
        pios_gpio->pullControlClock[0] = 0;
        pios_gpio->pullControlClock[1] = 0;
    }
    else
    {
        pios_gpio->pullControlEnable = PIOS_GPIO_PULL_OFF;
    }
}

void pios_gpio_pullControl ( uint32_t pin, uint32_t pull )
{
    if ( pin > 53 ) 
        return;

    uint32_t p[2]={0};
    uint32_t i = 0;
    if ( pin > 31 )
    {
        i++;
        pin-=32;
    }
    
    p[i] = (1 << pin);
    
    pios_gpio_pullBulk ( p, pull );    
}

/**
 * \brief reads the value of the GPIO-pin (set as input)
 * \param pin the GPIO pin you want the state of (in [0, 53])
 * \return the state of the GPIO-Pin (i.e. HIGH or LOW-signal level), 
 * always returns LOW if the input is invalid
 **/
uint32_t pios_gpio_read ( uint32_t pin )
{
    if ( pin > 53 ) 
        return PIOS_GPIO_LOW;

    volatile uint32_t* base = &pios_gpio->level[0];
    if ( pin > 31 )
    {
        base++;
        pin -= 32;
    }
    
    uint32_t val = *base;
    return ( ((val & (1<<pin)) == 0) ? PIOS_GPIO_LOW : PIOS_GPIO_HIGH );
}

/**
 * \brief writes a value to the GPIO-Pin (i.e. writes 1 to set or 1 to clear-registers)
 * \param pin GPIO-Pin (preferably between 0 and 53)
 * \param val output-value (1 for on, 0 for off)
 **/
void pios_gpio_write ( uint32_t pin, uint32_t val )
{
    if ( pin > 53 ) 
        return;

    volatile uint32_t* base = &pios_gpio->outputset[0];
    if ( val == PIOS_GPIO_LOW )
    {
        base = &pios_gpio->outputclear[0];
    }
    
    if ( pin > 31 )
    {
        base++;
        pin -= 32;
    }
    (*base) = 1 << pin;
}

/**
 * \brief sets the PIN into the mode we want
 * \param pin GPIO-Pin
 * \param val GPIO-Mode
 * \return NONE
 **/
void pios_gpio_pinmode ( uint32_t pin, uint32_t val )
{
    if ( val > 7 )
        return;

    if ( pin > 53 ) 
        return;
    
    volatile uint32_t *base = &pios_gpio->fn_select[0];
    while ( pin >= 10 )
    {
        base++;
        pin-=10;
    }
    
    uint32_t v = *base;
    v &= ~(7 << ((pin<<1) + pin));    /// move the 7 pin*3 times, so it masks the correct bits
    v |= (val << ((pin<<1) + pin));
    *base = v;
}

int pios_gpio_getPinmode ( uint32_t pin )
{
    if ( pin > 53 ) 
        return -1;
    
    volatile uint32_t *base = &pios_gpio->fn_select[0];
    while ( pin >= 10 )
    {
        base++;
        pin-=10;
    }
    
    uint32_t v = *base;
    v = v >> ((pin<<1)+pin) ;
    v &= 7;    /// move the 7 pin*3 times, so it masks the correct bits
    return v;
}
