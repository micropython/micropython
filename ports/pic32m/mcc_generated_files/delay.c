/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/* NOTICE: This module overwrites the CORE timer. */

#include <stdint.h>
#include <stdbool.h>

#include <xc.h>
//#include "clock.h"
#include "mcc_generated_files/mcc.h"

#define MILLISECONDS_TO_TICKS(milliseconds) (milliseconds*(_XTAL_FREQ/2)/1000)
#define MICROSECONDS_TO_TICKS(microseconds) (MILLISECONDS_TO_TICKS(microseconds)/1000)

static void delay_ticks(uint32_t ticks)
{
    IEC0bits.CTIE = 0;      //Disable core timer interrupt;
    
    _CP0_SET_COUNT(0);
    
    /* we are using the compare to test for a roll over in case of a really long
     * delay + an interrupt occurring close to the end causing it to roll over.
     * By setting the compare to the end of the timer and clearing the flag, we
     * can tell if the timer rolled over in that event. */
    _CP0_SET_COMPARE(0xFFFFFFFF);
    IFS0CLR = 0x00000001;
        
    while( (_CP0_GET_COUNT() < ticks ) && ( IFS0bits.CTIF == 0 ) )
    {
    }
}

void DELAY_milliseconds(uint16_t milliseconds)
{
    #if _XTAL_FREQ > 130000000
    #error This delay implementation will only work up until 130MHz
    #endif 

    delay_ticks(MILLISECONDS_TO_TICKS(milliseconds));
}

void DELAY_microseconds(uint16_t microseconds)
{
    delay_ticks(MICROSECONDS_TO_TICKS(microseconds));
}

