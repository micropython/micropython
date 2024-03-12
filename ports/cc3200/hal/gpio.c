//*****************************************************************************
//
//  gpio.c
//
//  Driver for the GPIO module.
//
//  Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup GPIO_General_Purpose_InputOutput_api
//! @{
//
//*****************************************************************************

#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_common_reg.h"
#include "debug.h"
#include "gpio.h"
#include "interrupt.h"


//*****************************************************************************
//
//! \internal
//! Checks a GPIO base address.
//!
//! \param ulPort is the base address of the GPIO port.
//!
//! This function determines if a GPIO port base address is valid.
//!
//! \return Returns \b true if the base address is valid and \b false
//! otherwise.
//
//*****************************************************************************
#if defined(DEBUG) && !defined(BOOTLOADER)
static tBoolean
GPIOBaseValid(unsigned long ulPort)
{
    return((ulPort == GPIOA0_BASE) ||
           (ulPort == GPIOA1_BASE) ||
           (ulPort == GPIOA2_BASE) ||
           (ulPort == GPIOA3_BASE) ||
           (ulPort == GPIOA4_BASE));
}
#else
#define GPIOBaseValid(ulPort)         (ulPort)
#endif

//*****************************************************************************
//
//! \internal
//! Gets the GPIO interrupt number.
//!
//! \param ulPort is the base address of the GPIO port.
//!
//! Given a GPIO base address, returns the corresponding interrupt number.
//!
//! \return Returns a GPIO interrupt number, or -1 if \e ulPort is invalid.
//
//*****************************************************************************
static long
GPIOGetIntNumber(unsigned long ulPort)
{
    unsigned int ulInt;

    //
    // Determine the GPIO interrupt number for the given module.
    //
    switch(ulPort)
    {
        case GPIOA0_BASE:
        {
            ulInt = INT_GPIOA0;
            break;
        }

        case GPIOA1_BASE:
        {
            ulInt = INT_GPIOA1;
            break;
        }

        case GPIOA2_BASE:
        {
            ulInt = INT_GPIOA2;
            break;
        }

        case GPIOA3_BASE:
        {
            ulInt = INT_GPIOA3;
            break;
        }

        default:
        {
            return(-1);
        }
    }

    //
    // Return GPIO interrupt number.
    //
    return(ulInt);
}

//*****************************************************************************
//
//! Sets the direction and mode of the specified pin(s).
//!
//! \param ulPort is the base address of the GPIO port
//! \param ucPins is the bit-packed representation of the pin(s).
//! \param ulPinIO is the pin direction and/or mode.
//!
//! This function will set the specified pin(s) on the selected GPIO port
//! as either an input or output under software control, or it will set the
//! pin to be under hardware control.
//!
//! The parameter \e ulPinIO is an enumerated data type that can be one of
//! the following values:
//!
//! - \b GPIO_DIR_MODE_IN
//! - \b GPIO_DIR_MODE_OUT
//!
//! where \b GPIO_DIR_MODE_IN specifies that the pin will be programmed as
//! a software controlled input, \b GPIO_DIR_MODE_OUT specifies that the pin
//! will be programmed as a software controlled output.
//!
//! The pin(s) are specified using a bit-packed byte, where each bit that is
//! set identifies the pin to be accessed, and where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \note GPIOPadConfigSet() must also be used to configure the corresponding
//! pad(s) in order for them to propagate the signal to/from the GPIO.
//!
//! \return None.
//
//*****************************************************************************
void
GPIODirModeSet(unsigned long ulPort, unsigned char ucPins,
               unsigned long ulPinIO)
{
    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ulPort));
    ASSERT((ulPinIO == GPIO_DIR_MODE_IN) || (ulPinIO == GPIO_DIR_MODE_OUT));

    //
    // Set the pin direction and mode.
    //
    HWREG(ulPort + GPIO_O_GPIO_DIR) = ((ulPinIO & 1) ?
                                  (HWREG(ulPort + GPIO_O_GPIO_DIR) | ucPins) :
                                  (HWREG(ulPort + GPIO_O_GPIO_DIR) & ~(ucPins)));
}

//*****************************************************************************
//
//! Gets the direction and mode of a pin.
//!
//! \param ulPort is the base address of the GPIO port.
//! \param ucPin is the pin number.
//!
//! This function gets the direction and control mode for a specified pin on
//! the selected GPIO port.  The pin can be configured as either an input or
//! output under software control, or it can be under hardware control.  The
//! type of control and direction are returned as an enumerated data type.
//!
//! \return Returns one of the enumerated data types described for
//! GPIODirModeSet().
//
//*****************************************************************************
unsigned long
GPIODirModeGet(unsigned long ulPort, unsigned char ucPin)
{
    unsigned long ulDir;

    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ulPort));

    //
    // Return the pin direction
    //
    ulDir = HWREG(ulPort + GPIO_O_GPIO_DIR);
    return(((ulDir & ucPin) ? 1 : 0));
}

//*****************************************************************************
//
//! Sets the interrupt type for the specified pin(s).
//!
//! \param ulPort is the base address of the GPIO port.
//! \param ucPins is the bit-packed representation of the pin(s).
//! \param ulIntType specifies the type of interrupt trigger mechanism.
//!
//! This function sets up the various interrupt trigger mechanisms for the
//! specified pin(s) on the selected GPIO port.
//!
//! The parameter \e ulIntType is an enumerated data type that can be one of
//! the following values:
//!
//! - \b GPIO_FALLING_EDGE
//! - \b GPIO_RISING_EDGE
//! - \b GPIO_BOTH_EDGES
//! - \b GPIO_LOW_LEVEL
//! - \b GPIO_HIGH_LEVEL
//!
//! The pin(s) are specified using a bit-packed byte, where each bit that is
//! set identifies the pin to be accessed, and where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \note In order to avoid any spurious interrupts, the user must
//! ensure that the GPIO inputs remain stable for the duration of
//! this function.
//!
//! \return None.
//
//*****************************************************************************
void
GPIOIntTypeSet(unsigned long ulPort, unsigned char ucPins,
               unsigned long ulIntType)
{
    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ulPort));
    ASSERT((ulIntType == GPIO_FALLING_EDGE) ||
           (ulIntType == GPIO_RISING_EDGE) || (ulIntType == GPIO_BOTH_EDGES) ||
           (ulIntType == GPIO_LOW_LEVEL) || (ulIntType == GPIO_HIGH_LEVEL));

    //
    // Set the pin interrupt type.
    //
    HWREG(ulPort + GPIO_O_GPIO_IBE) = ((ulIntType & 1) ?
                                  (HWREG(ulPort + GPIO_O_GPIO_IBE) | ucPins) :
                                  (HWREG(ulPort + GPIO_O_GPIO_IBE) & ~(ucPins)));
    HWREG(ulPort + GPIO_O_GPIO_IS) = ((ulIntType & 2) ?
                                 (HWREG(ulPort + GPIO_O_GPIO_IS) | ucPins) :
                                 (HWREG(ulPort + GPIO_O_GPIO_IS) & ~(ucPins)));
    HWREG(ulPort + GPIO_O_GPIO_IEV) = ((ulIntType & 4) ?
                                  (HWREG(ulPort + GPIO_O_GPIO_IEV) | ucPins) :
                                  (HWREG(ulPort + GPIO_O_GPIO_IEV) & ~(ucPins)));
}

//*****************************************************************************
//
//! Gets the interrupt type for a pin.
//!
//! \param ulPort is the base address of the GPIO port.
//! \param ucPin is the pin number.
//!
//! This function gets the interrupt type for a specified pin on the selected
//! GPIO port.  The pin can be configured as a falling edge, rising edge, or
//! both edge detected interrupt, or it can be configured as a low level or
//! high level detected interrupt.  The type of interrupt detection mechanism
//! is returned as an enumerated data type.
//!
//! \return Returns one of the enumerated data types described for
//! GPIOIntTypeSet().
//
//*****************************************************************************
unsigned long
GPIOIntTypeGet(unsigned long ulPort, unsigned char ucPin)
{
    unsigned long ulIBE, ulIS, ulIEV;

    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ulPort));

    //
    // Return the pin interrupt type.
    //
    ulIBE = HWREG(ulPort + GPIO_O_GPIO_IBE);
    ulIS = HWREG(ulPort + GPIO_O_GPIO_IS);
    ulIEV = HWREG(ulPort + GPIO_O_GPIO_IEV);
    return(((ulIBE & ucPin) ? 1 : 0) | ((ulIS & ucPin) ? 2 : 0) |
           ((ulIEV & ucPin) ? 4 : 0));
}

//*****************************************************************************
//
//! Enables the specified GPIO interrupts.
//!
//! \param ulPort is the base address of the GPIO port.
//! \param ulIntFlags is the bit mask of the interrupt sources to enable.
//!
//! This function enables the indicated GPIO interrupt sources.  Only the
//! sources that are enabled can be reflected to the processor interrupt;
//! disabled sources have no effect on the processor.
//!
//! The \e ulIntFlags parameter is the logical OR of any of the following:
//!
//! - \b GPIO_INT_DMA   - interrupt due to GPIO triggered DMA Done
//! - \b GPIO_INT_PIN_0 - interrupt due to activity on Pin 0.
//! - \b GPIO_INT_PIN_1 - interrupt due to activity on Pin 1.
//! - \b GPIO_INT_PIN_2 - interrupt due to activity on Pin 2.
//! - \b GPIO_INT_PIN_3 - interrupt due to activity on Pin 3.
//! - \b GPIO_INT_PIN_4 - interrupt due to activity on Pin 4.
//! - \b GPIO_INT_PIN_5 - interrupt due to activity on Pin 5.
//! - \b GPIO_INT_PIN_6 - interrupt due to activity on Pin 6.
//! - \b GPIO_INT_PIN_7 - interrupt due to activity on Pin 7.
//!
//! \return None.
//
//*****************************************************************************
void
GPIOIntEnable(unsigned long ulPort, unsigned long ulIntFlags)
{
    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ulPort));

    //
    // Enable the interrupts.
    //
    HWREG(ulPort + GPIO_O_GPIO_IM) |= ulIntFlags;
}

//*****************************************************************************
//
//! Disables the specified GPIO interrupts.
//!
//! \param ulPort is the base address of the GPIO port.
//! \param ulIntFlags is the bit mask of the interrupt sources to disable.
//!
//! This function disables the indicated GPIO interrupt sources.  Only the
//! sources that are enabled can be reflected to the processor interrupt;
//! disabled sources have no effect on the processor.
//!
//! The \e ulIntFlags parameter is the logical OR of any of the following:
//!
//! - \b GPIO_INT_DMA   - interrupt due to GPIO triggered DMA Done
//! - \b GPIO_INT_PIN_0 - interrupt due to activity on Pin 0.
//! - \b GPIO_INT_PIN_1 - interrupt due to activity on Pin 1.
//! - \b GPIO_INT_PIN_2 - interrupt due to activity on Pin 2.
//! - \b GPIO_INT_PIN_3 - interrupt due to activity on Pin 3.
//! - \b GPIO_INT_PIN_4 - interrupt due to activity on Pin 4.
//! - \b GPIO_INT_PIN_5 - interrupt due to activity on Pin 5.
//! - \b GPIO_INT_PIN_6 - interrupt due to activity on Pin 6.
//! - \b GPIO_INT_PIN_7 - interrupt due to activity on Pin 7.
//!
//! \return None.
//
//*****************************************************************************
void
GPIOIntDisable(unsigned long ulPort, unsigned long ulIntFlags)
{
    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ulPort));

    //
    // Disable the interrupts.
    //
    HWREG(ulPort + GPIO_O_GPIO_IM) &= ~(ulIntFlags);
}

//*****************************************************************************
//
//! Gets interrupt status for the specified GPIO port.
//!
//! \param ulPort is the base address of the GPIO port.
//! \param bMasked specifies whether masked or raw interrupt status is
//! returned.
//!
//! If \e bMasked is set as \b true, then the masked interrupt status is
//! returned; otherwise, the raw interrupt status will be returned.
//!
//! \return Returns the current interrupt status, enumerated as a bit field of
//! values described in GPIOIntEnable().
//
//*****************************************************************************
long
GPIOIntStatus(unsigned long ulPort, tBoolean bMasked)
{
    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ulPort));

    //
    // Return the interrupt status.
    //
    if(bMasked)
    {
        return(HWREG(ulPort + GPIO_O_GPIO_MIS));
    }
    else
    {
        return(HWREG(ulPort + GPIO_O_GPIO_RIS));
    }
}

//*****************************************************************************
//
//! Clears the interrupt for the specified pin(s).
//!
//! \param ulPort is the base address of the GPIO port.
//! \param ulIntFlags is a bit mask of the interrupt sources to be cleared.
//!
//! Clears the interrupt for the specified pin(s).
//!
//! The \e ulIntFlags parameter has the same definition as the \e ulIntFlags
//! parameter to GPIOIntEnable().
//!
//!
//! \return None.
//
//*****************************************************************************
void
GPIOIntClear(unsigned long ulPort, unsigned long ulIntFlags)
{
    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ulPort));

    //
    // Clear the interrupts.
    //
    HWREG(ulPort + GPIO_O_GPIO_ICR) = ulIntFlags;
}

//*****************************************************************************
//
//! Registers an interrupt handler for a GPIO port.
//!
//! \param ulPort is the base address of the GPIO port.
//! \param pfnIntHandler is a pointer to the GPIO port interrupt handling
//! function.
//!
//! This function will ensure that the interrupt handler specified by
//! \e pfnIntHandler is called when an interrupt is detected from the selected
//! GPIO port.  This function will also enable the corresponding GPIO interrupt
//! in the interrupt controller; individual pin interrupts and interrupt
//! sources must be enabled with GPIOIntEnable().
//!
//! \sa IntRegister() for important information about registering interrupt
//! handlers.
//!
//! \return None.
//
//*****************************************************************************
void
GPIOIntRegister(unsigned long ulPort, void (*pfnIntHandler)(void))
{
    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ulPort));

    //
    // Get the interrupt number associated with the specified GPIO.
    //
    ulPort = GPIOGetIntNumber(ulPort);

    //
    // Register the interrupt handler.
    //
    IntRegister(ulPort, pfnIntHandler);

    //
    // Enable the GPIO interrupt.
    //
    IntEnable(ulPort);
}

//*****************************************************************************
//
//! Removes an interrupt handler for a GPIO port.
//!
//! \param ulPort is the base address of the GPIO port.
//!
//! This function will unregister the interrupt handler for the specified
//! GPIO port.  This function will also disable the corresponding
//! GPIO port interrupt in the interrupt controller; individual GPIO interrupts
//! and interrupt sources must be disabled with GPIOIntDisable().
//!
//! \sa IntRegister() for important information about registering interrupt
//! handlers.
//!
//! \return None.
//
//*****************************************************************************
void
GPIOIntUnregister(unsigned long ulPort)
{
    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ulPort));

    //
    // Get the interrupt number associated with the specified GPIO.
    //
    ulPort = GPIOGetIntNumber(ulPort);

    //
    // Disable the GPIO interrupt.
    //
    IntDisable(ulPort);

    //
    // Unregister the interrupt handler.
    //
    IntUnregister(ulPort);
}

//*****************************************************************************
//
//! Reads the values present of the specified pin(s).
//!
//! \param ulPort is the base address of the GPIO port.
//! \param ucPins is the bit-packed representation of the pin(s).
//!
//! The values at the specified pin(s) are read, as specified by \e ucPins.
//! Values are returned for both input and output pin(s), and the value
//! for pin(s) that are not specified by \e ucPins are set to 0.
//!
//! The pin(s) are specified using a bit-packed byte, where each bit that is
//! set identifies the pin to be accessed, and where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \return Returns a bit-packed byte providing the state of the specified
//! pin, where bit 0 of the byte represents GPIO port pin 0, bit 1 represents
//! GPIO port pin 1, and so on.  Any bit that is not specified by \e ucPins
//! is returned as a 0.  Bits 31:8 should be ignored.
//
//*****************************************************************************
long
GPIOPinRead(unsigned long ulPort, unsigned char ucPins)
{
    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ulPort));

    //
    // Return the pin value(s).
    //
    return(HWREG(ulPort + (GPIO_O_GPIO_DATA + (ucPins << 2))));
}

//*****************************************************************************
//
//! Writes a value to the specified pin(s).
//!
//! \param ulPort is the base address of the GPIO port.
//! \param ucPins is the bit-packed representation of the pin(s).
//! \param ucVal is the value to write to the pin(s).
//!
//! Writes the corresponding bit values to the output pin(s) specified by
//! \e ucPins.  Writing to a pin configured as an input pin has no effect.
//!
//! The pin(s) are specified using a bit-packed byte, where each bit that is
//! set identifies the pin to be accessed, and where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \return None.
//
//*****************************************************************************
void
GPIOPinWrite(unsigned long ulPort, unsigned char ucPins, unsigned char ucVal)
{
    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ulPort));

    //
    // Write the pins.
    //
    HWREG(ulPort + (GPIO_O_GPIO_DATA + (ucPins << 2))) = ucVal;
}

//*****************************************************************************
//
//! Enables a GPIO port as a trigger to start a DMA transaction.
//!
//! \param ulPort is the base address of the GPIO port.
//!
//! This function enables a GPIO port to be used as a trigger to start a uDMA
//! transaction. The GPIO pin will still generate interrupts if the interrupt is
//! enabled for the selected pin.
//!
//! \return None.
//
//*****************************************************************************
void
GPIODMATriggerEnable(unsigned long ulPort)
{
    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ulPort));

    //
    // Set the pin as a DMA trigger.
    //
    if(ulPort == GPIOA0_BASE)
    {
      HWREG(COMMON_REG_BASE + COMMON_REG_O_APPS_GPIO_TRIG_EN) |= 0x1;
    }
    else if(ulPort == GPIOA1_BASE)
    {
      HWREG(COMMON_REG_BASE + COMMON_REG_O_APPS_GPIO_TRIG_EN) |= 0x2;
    }
    else if(ulPort == GPIOA2_BASE)
    {
      HWREG(COMMON_REG_BASE + COMMON_REG_O_APPS_GPIO_TRIG_EN) |= 0x4;
    }
    else if(ulPort == GPIOA3_BASE)
    {
      HWREG(COMMON_REG_BASE + COMMON_REG_O_APPS_GPIO_TRIG_EN) |= 0x8;
    }
}

//*****************************************************************************
//
//! Disables a GPIO port as a trigger to start a DMA transaction.
//!
//! \param ulPort is the base address of the GPIO port.
//!
//! This function disables a GPIO port to be used as a trigger to start a uDMA
//! transaction.  This function can be used to disable this feature if it was
//! enabled via a call to GPIODMATriggerEnable().
//!
//! \return None.
//
//*****************************************************************************
void
GPIODMATriggerDisable(unsigned long ulPort)
{
    //
    // Check the arguments.
    //
    ASSERT(GPIOBaseValid(ulPort));

    //
    // Set the pin as a DMA trigger.
    //
    if(ulPort == GPIOA0_BASE)
    {
      HWREG(COMMON_REG_BASE + COMMON_REG_O_APPS_GPIO_TRIG_EN) &= ~0x1;
    }
    else if(ulPort == GPIOA1_BASE)
    {
      HWREG(COMMON_REG_BASE + COMMON_REG_O_APPS_GPIO_TRIG_EN) &= ~0x2;
    }
    else if(ulPort == GPIOA2_BASE)
    {
      HWREG(COMMON_REG_BASE + COMMON_REG_O_APPS_GPIO_TRIG_EN) &= ~0x4;
    }
    else if(ulPort == GPIOA3_BASE)
    {
      HWREG(COMMON_REG_BASE + COMMON_REG_O_APPS_GPIO_TRIG_EN) &= ~0x8;
    }
}


//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
