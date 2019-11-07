/**
  NVM Generated Driver API Header File

  @Company
    Microchip Technology Inc.

  @File Name
    nvm.h

  @Summary
    This is the generated header file for the NVM driver using PIC32MX MCUs

  @Description
    This header file provides APIs for driver for NVM.
    Generation Information :
        Product Revision  :  PIC32MX MCUs - pic32mx : v1.35
        Device            :  PIC32MX170F256B
        Driver Version    :  2
    The generated drivers are tested against the following:
        Compiler          :  XC32 1.42
        MPLAB 	          :  MPLAB X 3.55
*/

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

#ifndef _NVM_H
#define _NVM_H

/**
  Section: Included Files
*/

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#ifdef __cplusplus  /* Provide C++ Compatibility */

    extern "C" {

#endif

#define NVMKEY1 0xAA996655
#define NVMKEY2 0x556699AA

#define NVM_OPERATION_WRITE_WORD 0x1
#define NVM_OPERATION_WRITE_ROW  0x3
#define NVM_OPERATION_ERASE_PAGE 0x4
#define NVM_OPERATION_ERASE_PFM  0x5
#define NVM_NO_OPERATION_MASK    0xF

#define NVM_WRITE_CONTROL_BIT             0x8000
#define NVM_WRITE_OR_ERASE_ENABLE_BIT     0x4000
#define NVM_WRERR_LVDERR_MASK             0x3000

/**
  Section: Interface Routines
*/

/**
  @Summary
    This function initializes NVM module

  @Description
    This routine initializes the NVM driver module
    This routine must be called before any other NVM routine is called.
	
  @Preconditions
    None.

  @Param
    None.

  @Returns
    None.

  @Example
    <code>
    uint32_t address = 0x1D01FFF0;
    uint32_t data = 0x01234567;

    NVM_Initialize();

    NVM_WriteWord( address, data );

    </code>

*/

void NVM_Initialize (void);
/**
  @Summary
    Maintains the driver's state machine and implements its ISR

  @Description
    This routine is used to maintain the driver's internal state
    machine and implement its ISR for interrupt-driven implementations.
  
  @Preconditions
    None.

  @Param
    None.

  @Returns
    None.

  @Example
    <code>
    while (true)
    {
        NVM_Tasks();

        // Do other tasks
    }
    </code>
    
*/

void NVM_Tasks( void );

/**
  @Summary
    Writes a word in given flash memory location.

  @Description
    This routine writes a data word in the given physical flash memory location.

  @Param
    address - physical address location from flash memory.
    data    - 32 bit value to be stored in the given flash location.	

  @Returns
    res - error bits after write word operation

  @Example 
    <code>
        uint32_t *address = 0x1D01FFF0;
        uint32_t data = 0x01234567;

        NVM_WriteWord( address, data );
    <code>
  	
*/

unsigned int NVM_WriteWord (void* address, unsigned int data);

/**
  @Summary
    Writes a row of data from SRAM to the given flash memory start location.

  @Description
    This routine writes a row of data from SRAM to the given flash memory start location.

  @Param
    address - physical address location from flash memory.
    data    - physical address location from sram memory.

  @Returns
    res - error bits after write row operation

  @Example 
    <code>
        uint32_t *address = 0x1D01FF00;
        uint32_t *data = 0x00007F00;

        NVM_WriteRow( address, data );
    <code>
  	
*/

unsigned int NVM_WriteRow (void* address, void* data);

/**
  @Summary
    Erases the given page from flash memory

  @Description
    This routine erases the given page from flash memory

  @Param
    address - physical address location from flash memory.

  @Returns
    res - error bits after page erase operation

  @Example 
    <code>
        uint32_t *address = 0x1D01FC00;

        NVM_ErasePage( address );
    <code>
  	
*/

unsigned int NVM_ErasePage(void* address);

/**
  @Summary
    Erases entire Program Flash Memory (PFM)

  @Description
    This routine erases entire Program Flash Memory

  @Param
    None.

  @Returns
    res - error bits after program flash memory erase operation.

  @Example 
    <code>

        NVM_ErasePFM();

    <code>
  	
*/

unsigned int NVM_ErasePFM(void);

/**
  @Summary
    Executes the unlock sequence to perform the given NVM operation.

  @Description
    This routine executes the unlock sequence to perform the NVM operation.

  @Param
    nvmop - type of operation.

  @Returns
    res - result value after any NVM operation

  @Example 
    <code>
        uint32_t nvmop = 0x4003;
        NVM_Unlock( nvmop );
    <code>
  	
*/

unsigned int NVM_Unlock (unsigned int nvmop);


#ifdef __cplusplus  /* Provide C++ Compatibility */

    }

#endif

#endif /* _NVM_H */

/**
 End of File
*/
