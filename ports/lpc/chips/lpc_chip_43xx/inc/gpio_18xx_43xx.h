/*
 * @brief LPC18xx/43xx GPIO driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#ifndef __GPIO_18XX_43XX_H_
#define __GPIO_18XX_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup GPIO_18XX_43XX CHIP: LPC18xx/43xx GPIO driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

/**
 * @brief  GPIO port register block structure
 */
typedef struct {				/*!< GPIO_PORT Structure */
	__IO uint8_t B[128][32];	/*!< Offset 0x0000: Byte pin registers ports 0 to n; pins PIOn_0 to PIOn_31 */
	__IO uint32_t W[32][32];	/*!< Offset 0x1000: Word pin registers port 0 to n */
	__IO uint32_t DIR[32];		/*!< Offset 0x2000: Direction registers port n */
	__IO uint32_t MASK[32];		/*!< Offset 0x2080: Mask register port n */
	__IO uint32_t PIN[32];		/*!< Offset 0x2100: Portpin register port n */
	__IO uint32_t MPIN[32];		/*!< Offset 0x2180: Masked port register port n */
	__IO uint32_t SET[32];		/*!< Offset 0x2200: Write: Set register for port n Read: output bits for port n */
	__O  uint32_t CLR[32];		/*!< Offset 0x2280: Clear port n */
	__O  uint32_t NOT[32];		/*!< Offset 0x2300: Toggle port n */
} LPC_GPIO_T;

/**
 * @brief	Initialize GPIO block
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @return	Nothing
 */
void Chip_GPIO_Init(LPC_GPIO_T *pGPIO);

/**
 * @brief	De-Initialize GPIO block
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @return	Nothing
 */
void Chip_GPIO_DeInit(LPC_GPIO_T *pGPIO);

/**
 * @brief	Set a GPIO port/bit state
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @param	port	: GPIO port to set
 * @param	pin		: GPIO pin to set
 * @param	setting	: true for high, false for low
 * @return	Nothing
 */
STATIC INLINE void Chip_GPIO_WritePortBit(LPC_GPIO_T *pGPIO, uint32_t port, uint8_t pin, bool setting)
{
	pGPIO->B[port][pin] = setting;
}

/**
 * @brief	Set a GPIO pin state via the GPIO byte register
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @param 	port	: GPIO Port number where @a pin is located
 * @param	pin		: GPIO pin to set
 * @param	setting	: true for high, false for low
 * @return	Nothing
 * @note	This function replaces Chip_GPIO_WritePortBit()
 */
STATIC INLINE void Chip_GPIO_SetPinState(LPC_GPIO_T *pGPIO, uint8_t port, uint8_t pin, bool setting)
{
	pGPIO->B[port][pin] = setting;
}

/**
 * @brief	Read a GPIO state
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @param	port	: GPIO port to read
 * @param	pin		: GPIO pin to read
 * @return	true of the GPIO is high, false if low
 * @note	It is recommended to use the Chip_GPIO_GetPinState() function instead.
 */
STATIC INLINE bool Chip_GPIO_ReadPortBit(LPC_GPIO_T *pGPIO, uint32_t port, uint8_t pin)
{
	return (bool) pGPIO->B[port][pin];
}

/**
 * @brief	Get a GPIO pin state via the GPIO byte register
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @param	port	: GPIO Port number where @a pin is located
 * @param	pin		: GPIO pin to get state for
 * @return	true if the GPIO is high, false if low
 * @note	This function replaces Chip_GPIO_ReadPortBit()
 */
STATIC INLINE bool Chip_GPIO_GetPinState(LPC_GPIO_T *pGPIO, uint8_t port, uint8_t pin)
{
	return (bool) pGPIO->B[port][pin];
}

/**
 * @brief	Set a GPIO direction
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @param	port	: GPIO port to set
 * @param	bit		: GPIO bit to set
 * @param	setting	: true for output, false for input
 * @return	Nothing
 * @note	It is recommended to use the Chip_GPIO_SetPinDIROutput(),
 * Chip_GPIO_SetPinDIRInput() or Chip_GPIO_SetPinDIR() functions instead
 * of this function.
 */
STATIC INLINE void Chip_GPIO_WriteDirBit(LPC_GPIO_T *pGPIO, uint32_t port, uint8_t bit, bool setting)
{
	if (setting) {
		pGPIO->DIR[port] |= 1UL << bit;
	}
	else {
		pGPIO->DIR[port] &= ~(1UL << bit);
	}
}

/**
 * @brief	Set GPIO direction for a single GPIO pin to an output
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @param	port	: GPIO Port number where @a pin is located
 * @param	pin		: GPIO pin to set direction on as output
 * @return	Nothing
 */
STATIC INLINE void Chip_GPIO_SetPinDIROutput(LPC_GPIO_T *pGPIO, uint8_t port, uint8_t pin)
{
	pGPIO->DIR[port] |= 1UL << pin;
}

/**
 * @brief	Set GPIO direction for a single GPIO pin to an input
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @param	port	: GPIO Port number where @a pin is located
 * @param	pin		: GPIO pin to set direction on as input
 * @return	Nothing
 */
STATIC INLINE void Chip_GPIO_SetPinDIRInput(LPC_GPIO_T *pGPIO, uint8_t port, uint8_t pin)
{
	pGPIO->DIR[port] &= ~(1UL << pin);
}

/**
 * @brief	Set GPIO direction for a single GPIO pin
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @param	port	: GPIO Port number where @a pin is located
 * @param	pin		: GPIO pin to set direction for
 * @param	output	: true for output, false for input
 * @return	Nothing
 */
STATIC INLINE void Chip_GPIO_SetPinDIR(LPC_GPIO_T *pGPIO, uint8_t port, uint8_t pin, bool output)
{
	if (output) {
		Chip_GPIO_SetPinDIROutput(pGPIO, port, pin);
	}
	else {
		Chip_GPIO_SetPinDIRInput(pGPIO, port, pin);
	}
}

/**
 * @brief	Read a GPIO direction (out or in)
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @param	port	: GPIO port to read
 * @param	bit		: GPIO bit to read
 * @return	true of the GPIO is an output, false if input
 * @note	It is recommended to use the Chip_GPIO_GetPinDIR() function instead.
 */
STATIC INLINE bool Chip_GPIO_ReadDirBit(LPC_GPIO_T *pGPIO, uint32_t port, uint8_t bit)
{
	return (bool) (((pGPIO->DIR[port]) >> bit) & 1);
}

/**
 * @brief	Get GPIO direction for a single GPIO pin
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @param	port	: GPIO Port number where @a pin is located
 * @param	pin		: GPIO pin to get direction for
 * @return	true if the GPIO is an output, false if input
 */
STATIC INLINE bool Chip_GPIO_GetPinDIR(LPC_GPIO_T *pGPIO, uint8_t port, uint8_t pin)
{
	return (bool) (((pGPIO->DIR[port]) >> pin) & 1);
}

/**
 * @brief	Set Direction for a GPIO port
 * @param	pGPIO		: The base of GPIO peripheral on the chip
 * @param	portNum		: port Number
 * @param	bitValue	: GPIO bit to set
 * @param	out			: Direction value, 0 = input, !0 = output
 * @return	None
 * @note	Bits set to '0' are not altered. It is recommended to use the
 * Chip_GPIO_SetPortDIR() function instead.
 */
STATIC INLINE void Chip_GPIO_SetDir(LPC_GPIO_T *pGPIO, uint8_t portNum, uint32_t bitValue, uint8_t out)
{
	if (out) {
		pGPIO->DIR[portNum] |= bitValue;
	}
	else {
		pGPIO->DIR[portNum] &= ~bitValue;
	}
}

/**
 * @brief	Set GPIO direction for a all selected GPIO pins to an output
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @param	port	: GPIO Port number where @a pin is located
 * @param	pinMask	: GPIO pin mask to set direction on as output (bits 0..b for pins 0..n)
 * @return	Nothing
 * @note	Sets multiple GPIO pins to the output direction, each bit's position that is
 * high sets the corresponding pin number for that bit to an output.
 */
STATIC INLINE void Chip_GPIO_SetPortDIROutput(LPC_GPIO_T *pGPIO, uint8_t port, uint32_t pinMask)
{
	pGPIO->DIR[port] |= pinMask;
}

/**
 * @brief	Set GPIO direction for a all selected GPIO pins to an input
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @param	port	: GPIO Port number where @a pin is located
 * @param	pinMask	: GPIO pin mask to set direction on as input (bits 0..b for pins 0..n)
 * @return	Nothing
 * @note	Sets multiple GPIO pins to the input direction, each bit's position that is
 * high sets the corresponding pin number for that bit to an input.
 */
STATIC INLINE void Chip_GPIO_SetPortDIRInput(LPC_GPIO_T *pGPIO, uint8_t port, uint32_t pinMask)
{
	pGPIO->DIR[port] &= ~pinMask;
}

/**
 * @brief	Set GPIO direction for a all selected GPIO pins to an input or output
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @param	port	: GPIO Port number where @a pin is located
 * @param	pinMask	: GPIO pin mask to set direction on (bits 0..b for pins 0..n)
 * @param	outSet	: Direction value, false = set as inputs, true = set as outputs
 * @return	Nothing
 * @note	Sets multiple GPIO pins to the input direction, each bit's position that is
 * high sets the corresponding pin number for that bit to an input.
 */
STATIC INLINE void Chip_GPIO_SetPortDIR(LPC_GPIO_T *pGPIO, uint8_t port, uint32_t pinMask, bool outSet)
{
	if (outSet) {
		Chip_GPIO_SetPortDIROutput(pGPIO, port, pinMask);
	}
	else {
		Chip_GPIO_SetPortDIRInput(pGPIO, port, pinMask);
	}
}

/**
 * @brief	Get GPIO direction for a all GPIO pins
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @param	port	: GPIO Port number where @a pin is located
 * @return	a bitfield containing the input and output states for each pin
 * @note	For pins 0..n, a high state in a bit corresponds to an output state for the
 * same pin, while a low  state corresponds to an input state.
 */
STATIC INLINE uint32_t Chip_GPIO_GetPortDIR(LPC_GPIO_T *pGPIO, uint8_t port)
{
	return pGPIO->DIR[port];
}

/**
 * @brief	Set GPIO port mask value for GPIO masked read and write
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @param	port	: port Number
 * @param	mask	: Mask value for read and write (only low bits are enabled)
 * @return	Nothing
 * @note	Controls which bits are set or unset when using the masked
 * GPIO read and write functions. A low state indicates the pin is settable
 * and readable via the masked write and read functions.
 */
STATIC INLINE void Chip_GPIO_SetPortMask(LPC_GPIO_T *pGPIO, uint8_t port, uint32_t mask)
{
	pGPIO->MASK[port] = mask;
}

/**
 * @brief	Get GPIO port mask value used for GPIO masked read and write
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @param	port	: port Number
 * @return	Returns value set with the Chip_GPIO_SetPortMask() function.
 * @note	A high bit in the return value indicates that that GPIO pin for the
 * port cannot be set using the masked write function.
 */
STATIC INLINE uint32_t Chip_GPIO_GetPortMask(LPC_GPIO_T *pGPIO, uint8_t port)
{
	return pGPIO->MASK[port];
}

/**
 * @brief	Set all GPIO raw pin states (regardless of masking)
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @param	port	: GPIO Port number where @a pin is located
 * @param	value	: Value to set all GPIO pin states (0..n) to
 * @return	Nothing
 */
STATIC INLINE void Chip_GPIO_SetPortValue(LPC_GPIO_T *pGPIO, uint8_t port, uint32_t value)
{
	pGPIO->PIN[port] = value;
}

/**
 * @brief	Get all GPIO raw pin states (regardless of masking)
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @param	port	: GPIO Port number where @a pin is located
 * @return	Current (raw) state of all GPIO pins
 */
STATIC INLINE uint32_t Chip_GPIO_GetPortValue(LPC_GPIO_T *pGPIO, uint8_t port)
{
	return pGPIO->PIN[port];
}

/**
 * @brief	Set all GPIO pin states, but mask via the MASKP0 register
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @param	port	: GPIO Port number where @a pin is located
 * @param	value	: Value to set all GPIO pin states (0..n) to
 * @return	Nothing
 */
STATIC INLINE void Chip_GPIO_SetMaskedPortValue(LPC_GPIO_T *pGPIO, uint8_t port, uint32_t value)
{
	pGPIO->MPIN[port] = value;
}

/**
 * @brief	Get all GPIO pin statesm but mask via the MASKP0 register
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @param	port	: GPIO Port number where @a pin is located
 * @return	Current (masked) state of all GPIO pins
 */
STATIC INLINE uint32_t Chip_GPIO_GetMaskedPortValue(LPC_GPIO_T *pGPIO, uint8_t port)
{
	return pGPIO->MPIN[port];
}

/**
 * @brief	Set a GPIO port/bit to the high state
 * @param	pGPIO		: The base of GPIO peripheral on the chip
 * @param	portNum		: port number
 * @param	bitValue	: bit(s) in the port to set high
 * @return	None
 * @note	Any bit set as a '0' will not have it's state changed. This only
 * applies to ports configured as an output. It is recommended to use the
 * Chip_GPIO_SetPortOutHigh() function instead.
 */
STATIC INLINE void Chip_GPIO_SetValue(LPC_GPIO_T *pGPIO, uint8_t portNum, uint32_t bitValue)
{
	pGPIO->SET[portNum] = bitValue;
}

/**
 * @brief	Set selected GPIO output pins to the high state
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @param	port	: GPIO Port number where @a pin is located
 * @param	pins	: pins (0..n) to set high
 * @return	None
 * @note	Any bit set as a '0' will not have it's state changed. This only
 * applies to ports configured as an output.
 */
STATIC INLINE void Chip_GPIO_SetPortOutHigh(LPC_GPIO_T *pGPIO, uint8_t port, uint32_t pins)
{
	pGPIO->SET[port] = pins;
}

/**
 * @brief	Set an individual GPIO output pin to the high state
 * @param	pGPIO	: The base of GPIO peripheral on the chip'
 * @param	port	: GPIO Port number where @a pin is located
 * @param	pin		: pin number (0..n) to set high
 * @return	None
 * @note	Any bit set as a '0' will not have it's state changed. This only
 * applies to ports configured as an output.
 */
STATIC INLINE void Chip_GPIO_SetPinOutHigh(LPC_GPIO_T *pGPIO, uint8_t port, uint8_t pin)
{
	pGPIO->SET[port] = (1 << pin);
}

/**
 * @brief	Set a GPIO port/bit to the low state
 * @param	pGPIO		: The base of GPIO peripheral on the chip
 * @param	portNum		: port number
 * @param	bitValue	: bit(s) in the port to set low
 * @return	None
 * @note	Any bit set as a '0' will not have it's state changed. This only
 * applies to ports configured as an output.
 */
STATIC INLINE void Chip_GPIO_ClearValue(LPC_GPIO_T *pGPIO, uint8_t portNum, uint32_t bitValue)
{
	pGPIO->CLR[portNum] = bitValue;
}

/**
 * @brief	Set selected GPIO output pins to the low state
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @param	port	: GPIO Port number where @a pin is located
 * @param	pins	: pins (0..n) to set low
 * @return	None
 * @note	Any bit set as a '0' will not have it's state changed. This only
 * applies to ports configured as an output.
 */
STATIC INLINE void Chip_GPIO_SetPortOutLow(LPC_GPIO_T *pGPIO, uint8_t port, uint32_t pins)
{
	pGPIO->CLR[port] = pins;
}

/**
 * @brief	Set an individual GPIO output pin to the low state
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @param	port	: GPIO Port number where @a pin is located
 * @param	pin		: pin number (0..n) to set low
 * @return	None
 * @note	Any bit set as a '0' will not have it's state changed. This only
 * applies to ports configured as an output.
 */
STATIC INLINE void Chip_GPIO_SetPinOutLow(LPC_GPIO_T *pGPIO, uint8_t port, uint8_t pin)
{
	pGPIO->CLR[port] = (1 << pin);
}

/**
 * @brief	Toggle selected GPIO output pins to the opposite state
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @param	port	: GPIO Port number where @a pin is located
 * @param	pins	: pins (0..n) to toggle
 * @return	None
 * @note	Any bit set as a '0' will not have it's state changed. This only
 * applies to ports configured as an output.
 */
STATIC INLINE void Chip_GPIO_SetPortToggle(LPC_GPIO_T *pGPIO, uint8_t port, uint32_t pins)
{
	pGPIO->NOT[port] = pins;
}

/**
 * @brief	Toggle an individual GPIO output pin to the opposite state
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @param	port	: GPIO Port number where @a pin is located
 * @param	pin		: pin number (0..n) to toggle
 * @return	None
 * @note	Any bit set as a '0' will not have it's state changed. This only
 * applies to ports configured as an output.
 */
STATIC INLINE void Chip_GPIO_SetPinToggle(LPC_GPIO_T *pGPIO, uint8_t port, uint8_t pin)
{
	pGPIO->NOT[port] = (1 << pin);
}

/**
 * @brief	Read current bit states for the selected port
 * @param	pGPIO	: The base of GPIO peripheral on the chip
 * @param	portNum	: port number to read
 * @return	Current value of GPIO port
 * @note	The current states of the bits for the port are read, regardless of
 * whether the GPIO port bits are input or output.
 */
STATIC INLINE uint32_t Chip_GPIO_ReadValue(LPC_GPIO_T *pGPIO, uint8_t portNum)
{
	return pGPIO->PIN[portNum];
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __GPIO_18XX_43XX_H_ */






