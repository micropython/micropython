#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_sysmpu.h"

void BOARD_InitHardware(void)
{
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitPins();
    SYSMPU_Enable(SYSMPU, false);
}