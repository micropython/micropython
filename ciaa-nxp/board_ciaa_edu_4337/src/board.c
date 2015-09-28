/*
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
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
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "board.h"
#include "string.h"

/** @ingroup BOARD_NGX_XPLORER_18304330
 * @{
 */

/* SDIO Data pin configuration bits */
#define SDIO_DAT_PINCFG (SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_PULLUP | SCU_MODE_FUNC7)

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/* System configuration variables used by chip driver */
const uint32_t ExtRateIn = 0;
const uint32_t OscRateIn = 12000000;

typedef struct {
	uint8_t port;
	uint8_t pin;
} io_port_t;

static const io_port_t gpioLEDBits[] = {{0, 14}, {1, 11}, {1, 12}, {5, 0}, {5, 1}, {5, 2}};

typedef struct {
	uint8_t* buffer;
	uint32_t size;
	uint32_t index;
	uint32_t finalByte;
	uint32_t timeout;
	uint32_t timeoutCounter;
	uint8_t flagNewPacket;
} UartRxBufferData;
static UartRxBufferData uart0RxBufferData;
static UartRxBufferData uart3RxBufferData;


typedef struct {
	void(*callbackSw)(void*);
	void* callbackSwArg;
} ButtonData;
static ButtonData buttonsData[4];

typedef struct {
	void(*callback)(void*);
        void* callbackArg;
	uint8_t gpioNumber;
} ExtIntData;
static ExtIntData extIntData[4];


//================================================[UART Management]==========================================================
void Board_UART_Init(LPC_USART_T *pUART)
{
	if(pUART==LPC_USART0)
	{
		/* UART0 (RS485/Profibus) */
   		Chip_UART_Init(LPC_USART0);
   		Chip_UART_SetBaud(LPC_USART0, 115200);

   		Chip_UART_SetupFIFOS(LPC_USART0, UART_FCR_FIFO_EN | UART_FCR_TRG_LEV0);

   		Chip_UART_TXEnable(LPC_USART0);

   		Chip_SCU_PinMux(9, 5, MD_PDN, FUNC7);              /* P9_5: UART0_TXD */
   		Chip_SCU_PinMux(9, 6, MD_PLN|MD_EZI|MD_ZI, FUNC7); /* P9_6: UART0_RXD */

   		Chip_UART_SetRS485Flags(LPC_USART0, UART_RS485CTRL_DCTRL_EN | UART_RS485CTRL_OINV_1);

   		Chip_SCU_PinMux(6, 2, MD_PDN, FUNC2);              /* P6_2: UART0_DIR */
		uart0RxBufferData.buffer = NULL;
		uart0RxBufferData.size=0;
		uart0RxBufferData.index=0;
		uart0RxBufferData.flagNewPacket=0;
		uart0RxBufferData.timeoutCounter=0;
        	//NVIC_SetPriority(USART0_IRQn, 1);
        	//NVIC_EnableIRQ(USART0_IRQn);

	}
	else if(pUART==LPC_USART3)
	{
		/* UART3 (RS232) */
   		Chip_UART_Init(LPC_USART3);
   		Chip_UART_SetBaud(LPC_USART3, 115200);

   		Chip_UART_SetupFIFOS(LPC_USART3, UART_FCR_FIFO_EN | UART_FCR_TRG_LEV0);

   		Chip_UART_TXEnable(LPC_USART3);

   		Chip_SCU_PinMux(2, 3, MD_PDN, FUNC2);              /* P2_3: UART3_TXD */
   		Chip_SCU_PinMux(2, 4, MD_PLN|MD_EZI|MD_ZI, FUNC2); /* P2_4: UART3_RXD */
		uart3RxBufferData.buffer = NULL;
                uart3RxBufferData.size=0;
                uart3RxBufferData.index=0;
		uart3RxBufferData.flagNewPacket=0;
                uart3RxBufferData.timeoutCounter=0;

                //NVIC_SetPriority(USART3_IRQn, 1);
                //NVIC_EnableIRQ(USART3_IRQn);
	}
	/* Restart FIFOS: set Enable, Reset content, set trigger level */
   	Chip_UART_SetupFIFOS(pUART, UART_FCR_FIFO_EN | UART_FCR_TX_RS | UART_FCR_RX_RS | UART_FCR_TRG_LEV0);
   	/* dummy read */
   	Chip_UART_ReadByte(pUART);
   	/* enable rx interrupt */
   	Chip_UART_IntEnable(pUART, UART_IER_RBRINT);
}

uint32_t Board_UART_Write(LPC_USART_T *pUART, uint8_t const * const buffer, uint32_t const size)
{
   uint32_t ret = 0;

   while(ret<size)
   {
       while((Chip_UART_ReadLineStatus(pUART) & UART_LSR_THRE) == 0){}

       Chip_UART_SendByte(pUART, buffer[ret]);
       /* bytes written */
       ret++;
   }
   return ret;
}

void Board_UART_setRxBuffer(LPC_USART_T *pUART,uint8_t* pBuffer,uint32_t size,uint32_t timeout, uint8_t finalByte)
{
    if(pUART==LPC_USART0)
    {
	uart0RxBufferData.buffer = pBuffer;
        uart0RxBufferData.size=size;
	uart0RxBufferData.timeout=timeout;
	uart0RxBufferData.finalByte=finalByte;
	NVIC_SetPriority(USART0_IRQn, 1);
        NVIC_EnableIRQ(USART0_IRQn);
    }else if(pUART==LPC_USART3)
    {
        uart3RxBufferData.buffer = pBuffer;
        uart3RxBufferData.size=size;
        uart3RxBufferData.timeout=timeout;
        uart3RxBufferData.finalByte=finalByte;
	NVIC_SetPriority(USART3_IRQn, 1);
        NVIC_EnableIRQ(USART3_IRQn);
    }
}

void Board_UART_setConfig(LPC_USART_T *pUART,int32_t baud,int32_t stopBits,int32_t parity)
{
	Chip_UART_SetBaud(pUART, baud);

	uint32_t config=UART_LCR_WLEN8;
	if(stopBits==2)
		config|=UART_LCR_SBS_2BIT;
	else
		config|=UART_LCR_SBS_1BIT;

	switch(parity)
	{
		case 1: config|=UART_LCR_PARITY_ODD; break;
		case 2: config|=UART_LCR_PARITY_EVEN; break;
		case 3: config|=UART_LCR_PARITY_F_0; break;
		case 4: config|=UART_LCR_PARITY_F_1; break;
		default: config|=UART_LCR_PARITY_DIS;
	}
	Chip_UART_ConfigData(pUART, config);

}

uint32_t Board_UART_isNewPacket(LPC_USART_T *pUART)
{
    if(pUART==LPC_USART0)
    {
        return uart0RxBufferData.flagNewPacket;
    }else if(pUART==LPC_USART3)
    {
        return uart3RxBufferData.flagNewPacket;
    }
    return 0;
}

uint32_t Board_UART_getRxSize(LPC_USART_T *pUART)
{
    if(pUART==LPC_USART0)
    {
        return uart0RxBufferData.index;
    }else if(pUART==LPC_USART3)
    {
        return uart3RxBufferData.index;
    }
    return 0;
}

void Board_UART_resetRx(LPC_USART_T *pUART)
{
    if(pUART==LPC_USART0)
    {
        uart0RxBufferData.flagNewPacket=0;
	uart0RxBufferData.timeoutCounter=0;
	uart0RxBufferData.index=0;
    }else if(pUART==LPC_USART3)
    {
        uart3RxBufferData.flagNewPacket=0;
        uart3RxBufferData.timeoutCounter=0;
        uart3RxBufferData.index=0;
    }
}

int32_t Board_UART_getChar(LPC_USART_T *pUART)
{
        if (Chip_UART_ReadLineStatus(pUART) & UART_LSR_RDR) {
                return (int32_t)Chip_UART_ReadByte(pUART);
        }
	return -1;
}

int32_t Board_UART_charAvailable(LPC_USART_T *pUART)
{
	return Chip_UART_ReadLineStatus(pUART) & UART_LSR_RDR;
}

void UART3_IRQHandler (void)
{
   uint8_t status = Chip_UART_ReadLineStatus(LPC_USART3);

   if(status & UART_LSR_RDR)
   {
	if(uart3RxBufferData.index<uart3RxBufferData.size)
	{
  	 	uart3RxBufferData.buffer[uart3RxBufferData.index] = Chip_UART_ReadByte(LPC_USART3);
		if(uart3RxBufferData.timeout==0)
		{
			if(uart3RxBufferData.buffer[uart3RxBufferData.index]==uart3RxBufferData.finalByte)
			{
				uart3RxBufferData.flagNewPacket=1;
				//Board_UARTPutSTR("entro paquete por byte final!:");
			        //Board_UARTPutSTR(uart3RxBufferData.buffer);
			}
		}
		else
		{
			uart3RxBufferData.timeoutCounter=uart3RxBufferData.timeout;
		}
		uart3RxBufferData.index++;
	}
	else
		uart3RxBufferData.flagNewPacket=1;
   }
}

void UART0_IRQHandler (void)
{
   uint8_t status = Chip_UART_ReadLineStatus(LPC_USART0);

   if(status & UART_LSR_RDR)
   {
        if(uart0RxBufferData.index<uart0RxBufferData.size)
        {
                uart0RxBufferData.buffer[uart0RxBufferData.index] = Chip_UART_ReadByte(LPC_USART0);
                if(uart0RxBufferData.timeout==0)
                {
                        if(uart0RxBufferData.buffer[uart0RxBufferData.index]==uart0RxBufferData.finalByte)
                        {
                                uart0RxBufferData.flagNewPacket=1;
                                //Board_UARTPutSTR("entro paquete por byte final!:");
                                //Board_UARTPutSTR(uart0RxBufferData.buffer);
                        }
                }
                else
                {
                        uart3RxBufferData.timeoutCounter=uart3RxBufferData.timeout;
                }
                uart3RxBufferData.index++;
        }
        else
                uart3RxBufferData.flagNewPacket=1;
   }
}

void Board_UART_tick_ms(void)
{
        if(uart0RxBufferData.flagNewPacket==0)
        {
                if(uart0RxBufferData.timeout!=0 && uart0RxBufferData.timeoutCounter>0)
                {
                        uart0RxBufferData.timeoutCounter--;
                        if(uart0RxBufferData.timeoutCounter==0)
                        {
                                uart0RxBufferData.flagNewPacket=1;
                                //Board_UARTPutSTR("entro paquete por timeout!:");
                                //Board_UARTPutSTR(uart0RxBufferData.buffer);
                        }
                }
        }

	if(uart3RxBufferData.flagNewPacket==0)
	{
		if(uart3RxBufferData.timeout!=0 && uart3RxBufferData.timeoutCounter>0)
		{
			uart3RxBufferData.timeoutCounter--;
			if(uart3RxBufferData.timeoutCounter==0)
			{
				uart3RxBufferData.flagNewPacket=1;
				//Board_UARTPutSTR("entro paquete por timeout!:");
                                //Board_UARTPutSTR(uart3RxBufferData.buffer);
			}
		}
	}
}
//===================================================================================================================================



/* Initialize debug output via UART for board */
void Board_Debug_Init(void)
{
#if defined(DEBUG_UART)
	Board_UART_Init(DEBUG_UART);

	Chip_UART_Init(DEBUG_UART);
	Chip_UART_SetBaudFDR(DEBUG_UART, 115200);
	Chip_UART_ConfigData(DEBUG_UART, UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS);

	/* Enable UART Transmit */
	Chip_UART_TXEnable(DEBUG_UART);
#endif
}

/* Sends a character on the UART */
void Board_UARTPutChar(char ch)
{
#if defined(DEBUG_UART)
	/* Wait for space in FIFO */
	while ((Chip_UART_ReadLineStatus(DEBUG_UART) & UART_LSR_THRE) == 0) {}
	Chip_UART_SendByte(DEBUG_UART, (uint8_t) ch);
#endif
}

/* Gets a character from the UART, returns EOF if no character is ready */
int Board_UARTGetChar(void)
{
#if defined(DEBUG_UART)
	if (Chip_UART_ReadLineStatus(DEBUG_UART) & UART_LSR_RDR) {
		return (int) Chip_UART_ReadByte(DEBUG_UART);
	}
#endif
	return EOF;
}

/* Outputs a string on the debug UART */
void Board_UARTPutSTR(const char *str)
{
#if defined(DEBUG_UART)
	while (*str != '\0') {
		Board_UARTPutChar(*str++);
	}
#endif
}

static void Board_LED_Init()
{
	uint32_t idx;

	for (idx = 0; idx < (sizeof(gpioLEDBits) / sizeof(io_port_t)); ++idx) {
		/* Set pin direction and init to off */
		Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, gpioLEDBits[idx].port, gpioLEDBits[idx].pin);
		Chip_GPIO_SetPinState(LPC_GPIO_PORT, gpioLEDBits[idx].port, gpioLEDBits[idx].pin, (bool) false);
	}
}

void Board_LED_Set(uint8_t LEDNumber, bool On)
{
	if (LEDNumber < (sizeof(gpioLEDBits) / sizeof(io_port_t)))
        Chip_GPIO_SetPinState(LPC_GPIO_PORT, gpioLEDBits[LEDNumber].port, gpioLEDBits[LEDNumber].pin, (bool) !On);
}

bool Board_LED_Test(uint8_t LEDNumber)
{
	if (LEDNumber < (sizeof(gpioLEDBits) / sizeof(io_port_t)))
		return (bool) !Chip_GPIO_GetPinState(LPC_GPIO_PORT, gpioLEDBits[LEDNumber].port, gpioLEDBits[LEDNumber].pin);

	return false;
}

void Board_LED_Toggle(uint8_t LEDNumber)
{
	Board_LED_Set(LEDNumber, !Board_LED_Test(LEDNumber));
}



void GPIO0_IRQHandler(void)
{
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(0));
	if(buttonsData[0].callbackSw!=NULL)
		buttonsData[0].callbackSw( buttonsData[0].callbackSwArg);
}

void GPIO1_IRQHandler(void)
{
        Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(1));
	if(buttonsData[1].callbackSw!=NULL)
		buttonsData[1].callbackSw( buttonsData[1].callbackSwArg);
}

void GPIO2_IRQHandler(void)
{
        Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(2));
	if(buttonsData[2].callbackSw!=NULL)
		buttonsData[2].callbackSw( buttonsData[2].callbackSwArg);
}

void GPIO3_IRQHandler(void)
{
        Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(3));
	if(buttonsData[3].callbackSw!=NULL)
		buttonsData[3].callbackSw( buttonsData[3].callbackSwArg);
}

void Board_Buttons_Init(void)
{
	 buttonsData[0].callbackSw=NULL;
	 buttonsData[1].callbackSw=NULL;
	 buttonsData[2].callbackSw=NULL;
	 buttonsData[3].callbackSw=NULL;

	Chip_SCU_PinMuxSet(0x1, 0, (SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC0));		// P1_0 as GPIO0[4]
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, BUTTONS_BUTTON1_GPIO_PORT_NUM, BUTTONS_BUTTON1_GPIO_BIT_NUM);	// input
	Chip_SCU_GPIOIntPinSel(0, 0, 4); // GPIO0[4] to INT0
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(0)); // INT0
        Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(0)); // INT0
        Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(0));   // INT0
	NVIC_ClearPendingIRQ(PIN_INT0_IRQn);
        NVIC_EnableIRQ(PIN_INT0_IRQn);


        Chip_SCU_PinMuxSet(0x1, 1, (SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC0));         // P1_1 as GPIO0[8]
        Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, BUTTONS_BUTTON2_GPIO_PORT_NUM, BUTTONS_BUTTON2_GPIO_BIT_NUM);   // input
	Chip_SCU_GPIOIntPinSel(1, 0, 8); // GPIO0[8] to INT1
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(1)); // INT1
        Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(1)); // INT1
        Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(1));   // INT1
	NVIC_ClearPendingIRQ(PIN_INT1_IRQn);
        NVIC_EnableIRQ(PIN_INT1_IRQn);


        Chip_SCU_PinMuxSet(0x1, 2, (SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC0));         // P1_2 as GPIO0[9]
        Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, BUTTONS_BUTTON3_GPIO_PORT_NUM, BUTTONS_BUTTON3_GPIO_BIT_NUM);   // input
	Chip_SCU_GPIOIntPinSel(2, 0, 9); // GPIO0[9] to INT2
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(2)); // INT2
        Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(2)); // INT2
        Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(2));   // INT2
        NVIC_ClearPendingIRQ(PIN_INT2_IRQn);
        NVIC_EnableIRQ(PIN_INT2_IRQn);


        Chip_SCU_PinMuxSet(0x1, 6, (SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC0));         // P1_6 as GPIO1[9]
        Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, BUTTONS_BUTTON4_GPIO_PORT_NUM, BUTTONS_BUTTON4_GPIO_BIT_NUM);   // input
	Chip_SCU_GPIOIntPinSel(3, 1, 9); // GPIO1[9] to INT3
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(3)); // INT3
        Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(3)); // INT3
        Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(3));   // INT3
        NVIC_ClearPendingIRQ(PIN_INT3_IRQn);
        NVIC_EnableIRQ(PIN_INT3_IRQn);
}

void Board_Buttons_configureCallback(int buttonNumber,void(*function)(void*),void* arg)
{
	buttonsData[buttonNumber].callbackSw = function;
	buttonsData[buttonNumber].callbackSwArg = arg;
}

int Buttons_GetStatusByNumber(int BUTTONNumber)
{
	switch(BUTTONNumber)
	{
	    case 0:
		if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, BUTTONS_BUTTON1_GPIO_PORT_NUM, BUTTONS_BUTTON1_GPIO_BIT_NUM) == 0) {
                	return 1;
        	}
        	return 0;
            case 1:
                if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, BUTTONS_BUTTON2_GPIO_PORT_NUM, BUTTONS_BUTTON2_GPIO_BIT_NUM) == 0) {
                        return 1;
                }
                return 0;

            case 2:
                if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, BUTTONS_BUTTON3_GPIO_PORT_NUM, BUTTONS_BUTTON3_GPIO_BIT_NUM) == 0) {
                        return 1;
                }
                return 0;

            case 3:
                if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, BUTTONS_BUTTON4_GPIO_PORT_NUM, BUTTONS_BUTTON4_GPIO_BIT_NUM) == 0) {
                        return 1;
                }
                return 0;

 	}
	return -1;
}

// GPIOs
typedef struct
{
    int8_t port;
    int8_t portBit;
    int8_t gpio;
    int8_t gpioBit;
    int8_t func;

}GpioInfo;

static GpioInfo gpiosInfo[]={
	{6,1,3,0,SCU_MODE_FUNC0},{6,4,3,3,SCU_MODE_FUNC0},{6,5,3,4,SCU_MODE_FUNC0},{6,7,5,15,SCU_MODE_FUNC4},{6,8,5,16,SCU_MODE_FUNC4},{6,9,3,5,SCU_MODE_FUNC0},{6,10,3,6,SCU_MODE_FUNC0},{6,11,3,7,SCU_MODE_FUNC0},{6,12,2,8,SCU_MODE_FUNC0}
};

void Board_GPIOs_Init(void)
{
	// GPIOs default: input. pull up and pull down disabled.
	for(int32_t i=0 ; i<9; i++)
        {
	    Chip_SCU_PinMuxSet(gpiosInfo[i].port, gpiosInfo[i].portBit, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | gpiosInfo[i].func));
  	    Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, gpiosInfo[i].gpio, gpiosInfo[i].gpioBit);
	}
	extIntData[0].callback=NULL;
	extIntData[1].callback=NULL;
	extIntData[2].callback=NULL;
	extIntData[3].callback=NULL;
}

void GPIO4_IRQHandler(void)
{
        Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(4));
        if(extIntData[0].callback!=NULL)
                extIntData[0].callback( extIntData[0].callbackArg);
}
void GPIO5_IRQHandler(void)
{
        Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(5));
        if(extIntData[1].callback!=NULL)
                extIntData[1].callback( extIntData[1].callbackArg);
}
void GPIO6_IRQHandler(void)
{
        Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(6));
        if(extIntData[2].callback!=NULL)
                extIntData[2].callback( extIntData[2].callbackArg);
}
void GPIO7_IRQHandler(void)
{
        Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(7));
        if(extIntData[3].callback!=NULL)
                extIntData[3].callback( extIntData[3].callbackArg);
}


void Board_GPIOs_configure(int32_t gpioNumber,int32_t mode, int32_t pullup)
{
	int32_t pullUpMode=SCU_MODE_INACT;
	switch(pullup)
	{
		case BOARD_GPIO_NOPULL:
			pullUpMode=SCU_MODE_INACT;
			break;
		case BOARD_GPIO_PULLUP:
			pullUpMode=SCU_MODE_PULLUP;
			break;
		case BOARD_GPIO_PULLDOWN:
			pullUpMode=SCU_MODE_PULLDOWN;
			break;
	}
	Chip_SCU_PinMuxSet(gpiosInfo[gpioNumber].port, gpiosInfo[gpioNumber].portBit, (pullUpMode | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | gpiosInfo[gpioNumber].func));

        switch(mode)
        {
                case BOARD_GPIO_MODE_INPUT:
                        Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, gpiosInfo[gpioNumber].gpio, gpiosInfo[gpioNumber].gpioBit);   // Input
                        break;
                case BOARD_GPIO_MODE_OUTPUT_PP:
                        Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, gpiosInfo[gpioNumber].gpio, gpiosInfo[gpioNumber].gpioBit); // Output
                        break;
                case BOARD_GPIO_MODE_OUTPUT_OD:
                        Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, gpiosInfo[gpioNumber].gpio, gpiosInfo[gpioNumber].gpioBit); // Output
                        break;
        }
}

uint32_t Board_GPIOs_readValue(int32_t gpioNumber)
{
	return Chip_GPIO_GetPinState(LPC_GPIO_PORT, gpiosInfo[gpioNumber].gpio, gpiosInfo[gpioNumber].gpioBit);
}

void Board_GPIOs_writeValue(int32_t gpioNumber,uint8_t value)
{
	if(value)
		Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, gpiosInfo[gpioNumber].gpio, gpiosInfo[gpioNumber].gpioBit);
	else
		Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, gpiosInfo[gpioNumber].gpio, gpiosInfo[gpioNumber].gpioBit);
}

bool Board_GPIOs_enableIntCallback(int gpioNumber,void(*function)(void*),void* arg, uint8_t flagEdgeLevel, uint8_t flagHighLow)
{
	// check if gpio alerady has assigned int
	for(uint8_t i=0; i<4 ; i++)
        {
                if(extIntData[i].callback!=NULL && extIntData[i].gpioNumber==gpioNumber)
                    return 0;
	}

	// find free extInt callback
	for(uint8_t i=0; i<4 ; i++)
	{
		if(extIntData[i].callback==NULL)
		{
        		extIntData[i].callback = function;
        		extIntData[i].callbackArg = arg;
			extIntData[i].gpioNumber=gpioNumber;
			// Enable interrupt
			uint8_t intNumber = i + 4; // starts from INT4
			Chip_SCU_GPIOIntPinSel(intNumber, gpiosInfo[gpioNumber].gpio, gpiosInfo[gpioNumber].gpioBit);
        		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(intNumber));

			if(flagEdgeLevel)
        			Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(intNumber));
			else
				Chip_PININT_SetPinModeLevel(LPC_GPIO_PIN_INT, PININTCH(intNumber));

			if(flagHighLow)
				Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(intNumber));
			else
        			Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(intNumber));

			switch(intNumber) {
				case 4: NVIC_ClearPendingIRQ(PIN_INT4_IRQn); NVIC_EnableIRQ(PIN_INT4_IRQn); break;
				case 5: NVIC_ClearPendingIRQ(PIN_INT5_IRQn); NVIC_EnableIRQ(PIN_INT5_IRQn); break;
				case 6: NVIC_ClearPendingIRQ(PIN_INT6_IRQn); NVIC_EnableIRQ(PIN_INT6_IRQn); break;
				case 7: NVIC_ClearPendingIRQ(PIN_INT7_IRQn); NVIC_EnableIRQ(PIN_INT7_IRQn); break;
			}
			return 1;
		}
	}
	return 0;
}

void Board_GPIOs_disableIntCallback(int gpioNumber)
{
	for(uint8_t i=0; i<4 ; i++)
        {
                if(extIntData[i].callback!=NULL && extIntData[i].gpioNumber==gpioNumber)
                {
			extIntData[i].callback=NULL;
			uint8_t intNumber = i + 4; // starts from INT4
			Chip_PININT_DisableIntHigh(LPC_GPIO_PIN_INT, PININTCH(intNumber));
			Chip_PININT_DisableIntLow(LPC_GPIO_PIN_INT, PININTCH(intNumber));
		}
        }
}

//_____________________________________________________________________________________________________________________________________________


void Board_Joystick_Init(void)
{}

uint8_t Joystick_GetStatus(void)
{
	return NO_BUTTON_PRESSED;
}

/* Returns the MAC address assigned to this board */
void Board_ENET_GetMacADDR(uint8_t *mcaddr)
{
	uint8_t boardmac[] = {0x00, 0x60, 0x37, 0x12, 0x34, 0x56};

	memcpy(mcaddr, boardmac, 6);
}

/* Set up and initialize all required blocks and functions related to the
   board hardware */
void Board_Init(void)
{
	/* Sets up DEBUG UART */
	DEBUGINIT();

	/* Initializes GPIO */
	Chip_GPIO_Init(LPC_GPIO_PORT);
	Board_GPIOs_Init();

	/* Initialize LEDs */
	Board_LED_Init();

	/* Initialize uarts */
	Board_UART_Init(LPC_USART0); //RS 485
	Board_UART_Init(LPC_USART3); // rs232

	/* Initialize buttons */
	Board_Buttons_Init();

	Chip_ENET_RMIIEnable(LPC_ETHERNET);
}

void Board_I2C_Init(I2C_ID_T id)
{
	if (id == I2C1) {
		/* Configure pin function for I2C1*/
		Chip_SCU_PinMuxSet(0x2, 3, (SCU_MODE_ZIF_DIS | SCU_MODE_INBUFF_EN | SCU_MODE_FUNC1));		/* P2.3 : I2C1_SDA */
		Chip_SCU_PinMuxSet(0x2, 4, (SCU_MODE_ZIF_DIS | SCU_MODE_INBUFF_EN | SCU_MODE_FUNC1));		/* P2.4 : I2C1_SCL */
	}
	else {
		Chip_SCU_I2C0PinConfig(I2C0_STANDARD_FAST_MODE);
	}
}

void Board_SDMMC_Init(void)
{
	Chip_SCU_PinMuxSet(0x1, 9, SDIO_DAT_PINCFG);	/* P1.9 connected to SDIO_D0 */
	Chip_SCU_PinMuxSet(0x1, 10, SDIO_DAT_PINCFG);	/* P1.10 connected to SDIO_D1 */
	Chip_SCU_PinMuxSet(0x1, 11, SDIO_DAT_PINCFG);	/* P1.11 connected to SDIO_D2 */
	Chip_SCU_PinMuxSet(0x1, 12, SDIO_DAT_PINCFG);	/* P1.12 connected to SDIO_D3 */

	Chip_SCU_ClockPinMuxSet(2, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_FUNC4));	/* CLK2 connected to SDIO_CLK */
	Chip_SCU_PinMuxSet(0x1, 6, SDIO_DAT_PINCFG);	/* P1.6 connected to SDIO_CMD */
	Chip_SCU_PinMuxSet(0x1, 13, (SCU_MODE_INBUFF_EN | SCU_MODE_FUNC7));	/* P1.13 connected to SDIO_CD */
}

void Board_SSP_Init(LPC_SSP_T *pSSP)
{
	if (pSSP == LPC_SSP1) {
		Chip_SCU_PinMuxSet(0x1, 5, (SCU_PINIO_FAST | SCU_MODE_FUNC5));  /* P1.5 => SSEL1 */
		Chip_SCU_PinMuxSet(0xF, 4, (SCU_PINIO_FAST | SCU_MODE_FUNC0));  /* PF.4 => SCK1 */

		Chip_SCU_PinMuxSet(0x1, 4, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC5)); /* P1.4 => MOSI1 */
		Chip_SCU_PinMuxSet(0x1, 3, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC5)); /* P1.3 => MISO1 */
	}
	else {
		return;
	}
}

/* Initialize DAC interface for the board */
void Board_DAC_Init(LPC_DAC_T *pDAC)
{
	Chip_SCU_DAC_Analog_Config();
}

#if 0
/* Initialize Audio Codec */
static Status Board_Audio_CodecInit(int micIn)
{

	if (!WM8904_Init(micIn)){
		return ERROR;
	}

	return SUCCESS;
}

/* Board Audio initialization */
void Board_Audio_Init(LPC_I2S_T *pI2S, int micIn)
{

	if (pI2S == LPC_I2S0) {
		/* TODO :Add pin mux for I2S0 later */
	} else if (pI2S == LPC_I2S1) {
		Chip_SCU_PinMuxSet (0x1, 19, (SCU_PINIO_FAST | SCU_MODE_FUNC7)); /* I2S1_TX_SCK */
		Chip_SCU_PinMuxSet (0x0, 1,  (SCU_PINIO_FAST | SCU_MODE_FUNC7)); /* I2S1_TX_SDA */
		Chip_SCU_PinMuxSet (0x3, 4,  (SCU_PINIO_FAST | SCU_MODE_FUNC6)); /* I2S1_RX_SDA */
		Chip_SCU_PinMuxSet (0x0, 0,  (SCU_PINIO_FAST | SCU_MODE_FUNC7)); /* I2S1_TX_WS */

        Chip_SCU_ClockPinMuxSet(0,(SCU_MODE_FUNC1|SCU_MODE_INACT));
		/* Setup base clock for CLKOUT */
		Chip_Clock_SetBaseClock(CLK_BASE_OUT, CLKIN_CRYSTAL, false, false);
	} else {
		/* It is a BUG catch it */
		while(1);
	}
	/* Init WM8904 CODEC */
	while (Board_Audio_CodecInit(micIn) != SUCCESS) {}
}

/* Initialize Pin Muxing for LCD */
void Board_LCD_Init(void)
{
	uint32_t val;

	Board_SSP_Init(LCD_SSP);
	val = LCD_SSP->CR0 & 0xFFFF;
	Chip_SCU_PinMuxSet(LCD_CDM_PORT, LCD_CMD_PIN, LCD_CMD_CFG);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, LCD_CMD_GPIO_PORT, LCD_CMD_GPIO_PIN);
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, LCD_CMD_GPIO_PORT, LCD_CMD_GPIO_PIN);

	/* Enable the SSP interface */
	Chip_SSP_Init(LCD_SSP);
	Chip_SSP_Set_Mode(LCD_SSP, SSP_MODE_MASTER);
	Chip_SSP_SetFormat(LCD_SSP, SSP_BITS_8, SSP_FRAMEFORMAT_SPI, SSP_CLOCK_CPHA1_CPOL1);
	Chip_SSP_SetBitRate(LCD_SSP, LCD_BIT_RATE);
	Chip_SSP_Enable(LCD_SSP);

	lcd_cfg_val = LCD_SSP->CR0 & 0xFFFF;
	LCD_SSP->CR0 = val;
}

/* Write data to LCD module */
void Board_LCD_WriteData(const uint8_t *data, uint16_t size)
{
	uint32_t val = LCD_SSP->CR0 & 0xFFFF;
	LCD_SSP->CR0 = lcd_cfg_val;
	Chip_SSP_WriteFrames_Blocking(LCD_SSP, data, size);
	LCD_SSP->CR0 = val;
}
#endif

/**
 * @}
 */
