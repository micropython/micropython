
/**
  NVM Generated Driver API Source File

  @Company
    Microchip Technology Inc.

  @File Name
    nvm.c

  @Summary
    This is the generated source file for the NVM driver using PIC32MX MCUs

  @Description
    This source file provides APIs for driver for NVM.
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

/**
  Section: Included Files
*/
#include <xc.h>
#include "nvm.h"

void NVM_Initialize (void)
{




}

void NVM_Tasks( void )
{
    if(IFS0bits.FCEIF)
    {
        IFS0CLR= 1 << _IFS0_FCEIF_POSITION;
    }
}

unsigned int NVM_WriteWord (void* address, unsigned int data)
{
    unsigned int res = 0;

    NVMDATA = data;
    /* convert virtual address to physical address and load into NVMADDR register */
    NVMADDR = ((unsigned int) address & 0x1FFFFFFF);
    /* Unlock and Perform the NVM operation */
    res = NVM_Unlock (NVM_OPERATION_WRITE_WORD);

    return res;
}

unsigned int NVM_WriteRow (void* address, void* data)
{
    unsigned int res = 0;

    NVMADDR = ((unsigned int) address & 0x1FFFFFFF);
    NVMSRCADDR = ((unsigned int) data & 0x1FFFFFFF);

    res = NVM_Unlock(NVM_OPERATION_WRITE_ROW);

    return res;
}

unsigned int NVM_ErasePage(void* address)
{
    unsigned int res = 0;

    NVMADDR = ((unsigned int) address & 0x1FFFFFFF);

    res = NVM_Unlock(NVM_OPERATION_ERASE_PAGE);

    return res;
}

unsigned int NVM_ErasePFM(void)
{
    unsigned int res = 0;

    res = NVM_Unlock(NVM_OPERATION_ERASE_PFM);

    return res;
}

unsigned int NVM_Unlock (unsigned int nvmop)
{
    unsigned int status = 0;
    /* Suspend or Disable all Interrupts */
    asm volatile ("di %0" : "=r" (status));

    /* clearing error bits before performing an NVM operation */
    NVMCONCLR = NVM_NO_OPERATION_MASK;

    /* Enable Flash Write/Erase Operations and Select Flash operation to perform */
    NVMCON = (NVM_WRITE_OR_ERASE_ENABLE_BIT | nvmop);

    NVMKEY = NVMKEY1;
    NVMKEY = NVMKEY2;

    /* Start the operation using the Set Register */
    NVMCONSET = NVM_WRITE_CONTROL_BIT;

    /* Wait for operation to complete */
    while (NVMCON & NVM_WRITE_CONTROL_BIT);

    /* Restore Interrupts */
    if (status & 0x00000001) {
    	asm volatile ("ei");
    } else {
    	asm volatile ("di");
    }

    /* Disable NVM write enable */
    NVMCONCLR = NVM_WRITE_OR_ERASE_ENABLE_BIT;

    /* Return WRERR and LVDERR Error Status Bits */
    return (NVMCON & NVM_WRERR_LVDERR_MASK);
}
