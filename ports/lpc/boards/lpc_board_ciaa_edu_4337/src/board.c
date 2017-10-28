#include "board.h"

#include <stdint.h>

/* System configuration variables used by chip driver */
const uint32_t ExtRateIn = 0;
const uint32_t OscRateIn = 12000000;

/* Structure for initial base clock states */
struct CLK_BASE_STATES {
    CHIP_CGU_BASE_CLK_T clk;    /* Base clock */
    CHIP_CGU_CLKIN_T clkin; /* Base clock source, see UM for allowable souorces per base clock */
    bool autoblock_enab;/* Set to true to enable autoblocking on frequency change */
    bool powerdn;       /* Set to true if the base clock is initially powered down */
};

/* Initial base clock states are mostly on */
static const struct CLK_BASE_STATES InitClkStates[] = {
    /* Ethernet Clock base */
    {CLK_BASE_PHY_TX, CLKIN_ENET_TX, true, false},
    {CLK_BASE_PHY_RX, CLKIN_ENET_TX, true, false},

    /* Clocks derived from dividers */
    {CLK_BASE_USB0, CLKIN_IDIVD, true, true}
};

static const PINMUX_GRP_T pinmuxing[] = {
    /* Board LEDs */
    {2, 10, (SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | SCU_MODE_FUNC0)},
    {2, 11, (SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | SCU_MODE_FUNC0)},
    {2, 12, (SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | SCU_MODE_FUNC0)},
    {2, 0, (SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | SCU_MODE_FUNC4)},
    {2, 1, (SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | SCU_MODE_FUNC4)},
    {2, 2, (SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | SCU_MODE_FUNC4)},

    /* UART 3 */
    {2, 3, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC2)},
    {2, 4, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC2)},

    /* UART 2 */
    {7, 1, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC6)},
    {7, 2, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC6)},

    /* CAN1 */
    {3, 2, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC2)},
    {3, 1, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC2)},

    /* GPIO2 */
    {4, 0, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC0)},
    {4, 1, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC0)},
    {4, 2, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC0)},
    {4, 3, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC0)},
    {4, 5, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC0)},
    {4, 6, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC0)},
    {4, 7, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC0)},

    /* GPIO5 */
    {4, 8, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC4)},
    {4, 9, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC4)},
    {4, 10, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC4)},

    /* ENET Pin mux (RMII Pins) */
    {1, 15, (SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC3)}, /* RXD0 */
    {1, 16, (SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC7)}, /* CRS_DV */
    {1, 17, (SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC3)}, /* MDIO */
    {1, 18, (SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC3)}, /* TXD0 */
    {1, 19, (SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC0)}, /* REFCLK */
    {1, 20, (SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC3)}, /* TXD1 */
    {7,  7, (SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC6)}, /* MDC */
    {0,  0, (SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC2)},  /* RXD1 */
    {0,  1, (SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INACT | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC6)}, /* TXEN */
};

void Board_UART_Init(LPC_USART_T *pUART)
{
   Chip_SCU_PinMuxSet(0x7, 1, (SCU_MODE_INACT | SCU_MODE_FUNC6));                  /* P7.1 : UART2_TXD */
   Chip_SCU_PinMuxSet(0x7, 2, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC6));/* P7.2 : UART2_RXD */
}

/* Initialize debug output via UART for board */
void Board_Debug_Init(void)
{
   Board_UART_Init(CONSOLE_UART);
   
   Chip_UART_Init(CONSOLE_UART);
   Chip_UART_SetBaudFDR(CONSOLE_UART, 115200);
   Chip_UART_ConfigData(CONSOLE_UART, UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS);
   /* Enable UART Transmit */
   Chip_UART_TXEnable(CONSOLE_UART);
}

/* Set up and initialize all required blocks and functions related to the
   board hardware */
void Board_Init(void)
{
   /* Sets up DEBUG UART */
   Board_Debug_Init();

   /* Initializes GPIO */
   Chip_GPIO_Init(LPC_GPIO_PORT);
}

/* Sets up system pin muxing */
void Board_SetupMuxing(void)
{
   /* Setup system level pin muxing */
   Chip_SCU_SetPinMuxing(pinmuxing, sizeof(pinmuxing) / sizeof(PINMUX_GRP_T));
   /* Off all leds */
#define X(port, pin) do { \
   Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, port, pin); \
   Chip_GPIO_SetPinState(LPC_GPIO_PORT, port, pin, false); \
} while(0)
    X(5, 0);
    X(5, 1);
    X(5, 2);
    X(0, 14);
    X(1, 11);
    X(1, 12);
#undef X
}

/* Set up and initialize clocking prior to call to main */
void Board_SetupClocking(void)
{
   int i;

   /* Enable Flash acceleration and setup wait states */
   Chip_CREG_SetFlashAcceleration(MAX_CLOCK_FREQ);

   /* Setup System core frequency to MAX_CLOCK_FREQ */
   Chip_SetupCoreClock(CLKIN_CRYSTAL, MAX_CLOCK_FREQ, true);

   /* Setup system base clocks and initial states. This won't enable and
      disable individual clocks, but sets up the base clock sources for
      each individual peripheral clock. */
   for (i = 0; i < (sizeof(InitClkStates) / sizeof(InitClkStates[0])); i++) {
       Chip_Clock_SetBaseClock(InitClkStates[i].clk, InitClkStates[i].clkin,
                               InitClkStates[i].autoblock_enab, InitClkStates[i].powerdn);
   }

   /* Reset and enable 32Khz oscillator */
   LPC_CREG->CREG0 &= ~((1 << 3) | (1 << 2));
   LPC_CREG->CREG0 |= (1 << 1) | (1 << 0);

   /* Wait until stable */
   volatile unsigned int delay = 10000;
   while (delay--);
}

/* Set up and initialize hardware prior to call to main */
void Board_SystemInit(void)
{
   /* Setup system clocking and memory. This is done early to allow the
      application and tools to clear memory and use scatter loading to
      external memory. */
   fpuInit();
   Board_SetupMuxing();
   Board_SetupClocking();
}
