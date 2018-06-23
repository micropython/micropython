#include "py/mphal.h"

void CLICKER2_board_early_init(void) {

    #define BATSTAT pin_B4
    #define FAULT	pin_C6
    #define SENSEL	pin_B12
    #define VSENSE	pin_C5

	mp_hal_pin_input(BATSTAT);	// charging status (low active)
	mp_hal_pin_input(FAULT);	// fault status of PMIC LTC3586-2 (low active)
    mp_hal_pin_input(VSENSE);	// analog input of battery voltage

    mp_hal_pin_output(SENSEL);	// selecting the shunt over th battery for voltage measurement
    mp_hal_pin_high(SENSEL);	// off
}
