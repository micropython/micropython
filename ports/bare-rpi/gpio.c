#include "gpio.h"

volatile pios_gpio_t* const pios_gpio = (volatile pios_gpio_t* const) (PIOS_IO_BASE + PIOS_GPIO_BASE);

void __attribute__((optimize("O0"))) wait ( int sec ) {    
    while ( sec > 0 ) {
        int counter = 0x400000;
        while ( counter > 0 )
            counter--; 
        sec--;
    }
}

void __attribute__((optimize("O1"))) pios_wasteCycles ( uint32_t cycles ) {
    cycles++;
    while ( cycles > 0 ) { 
        cycles--; 
    }
}

void pios_gpio_pullBulk ( uint32_t p[2], uint32_t pull ) {
    if ( pull == PIOS_GPIO_PULL_UP || 
         pull == PIOS_GPIO_PULL_DOWN || 
         pull == PIOS_GPIO_PULL_OFF) {
        pios_gpio->pullControlEnable = pull;
        pios_wasteCycles ( 150 );
        
        pios_gpio->pullControlClock[0] = p[0];
        pios_gpio->pullControlClock[1] = p[1];
        pios_wasteCycles ( 150 );
        
        //pios_gpio->pullControlEnable = PIOS_GPIO_PULL_OFF;
        pios_gpio->pullControlClock[0] = 0;
        pios_gpio->pullControlClock[1] = 0;
    }
    else {
        pios_gpio->pullControlEnable = PIOS_GPIO_PULL_OFF;
    }
}

void pios_gpio_pullControl ( uint32_t pin, uint32_t pull ) {
    if ( pin > 53 ) 
        return;

    uint32_t p[2]={0};
    uint32_t i = 0;
    if ( pin > 31 ) {
        i++;
        pin-=32;
    }
    
    p[i] = (1 << pin);
    
    pios_gpio_pullBulk ( p, pull );    
}

uint32_t pios_gpio_read ( uint32_t pin ) {
    if ( pin > 53 ) 
        return PIOS_GPIO_LOW;

    volatile uint32_t* base = &pios_gpio->level[0];
    if ( pin > 31 ) {
        base++;
        pin -= 32;
    }
    
    uint32_t val = *base;
    return ( ((val & (1<<pin)) == 0) ? PIOS_GPIO_LOW : PIOS_GPIO_HIGH );
}

void pios_gpio_write ( uint32_t pin, uint32_t val ) {
    if ( pin > 53 ) 
        return;

    volatile uint32_t* base = &pios_gpio->outputset[0];
    if ( val == PIOS_GPIO_LOW ) {
        base = &pios_gpio->outputclear[0];
    }
    
    if ( pin > 31 ) {
        base++;
        pin -= 32;
    }
    (*base) = 1 << pin;
}

void pios_gpio_pinmode ( uint32_t pin, uint32_t val ) {
    if ( val > 7 )
        return;

    if ( pin > 53 ) 
        return;
    
    volatile uint32_t *base = &pios_gpio->fn_select[0];
    while ( pin >= 10 ) {
        base++;
        pin-=10;
    }
    
    uint32_t v = *base;
    v &= ~(7 << ((pin<<1) + pin));    /// move the 7 pin*3 times, so it masks the correct bits
    v |= (val << ((pin<<1) + pin));
    *base = v;
}

int pios_gpio_getPinmode ( uint32_t pin ) {
    if ( pin > 53 ) 
        return -1;
    
    volatile uint32_t *base = &pios_gpio->fn_select[0];
    while ( pin >= 10 ) {
        base++;
        pin-=10;
    }
    
    uint32_t v = *base;
    v = v >> ((pin<<1)+pin) ;
    v &= 7;    /// move the 7 pin*3 times, so it masks the correct bits
    return v;
}
