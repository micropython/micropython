#include <stdint.h>
#include "app_error.h"


void
#ifdef DEBUG
app_error_handler(ret_code_t error_code, uint32_t line_num, const uint8_t * p_file_name)
#else
app_error_handler_bare(ret_code_t error_code)
#endif
{
#ifdef DEBUG
	for (;;) {
		/* FOREVER */
	}
#else
    NVIC_SystemReset();
#endif
}
