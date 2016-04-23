/*
 * panic.h
 *
 *  Created on: Jan 31, 2014
 *      Author: david_s5
 */
// The panic module will send SOS followed by 900 ms delay
// followed by 300 blinks of the value of code
/// ...---...    code   ...---...


#ifndef PANIC_H_
#define PANIC_H_
#ifdef __cplusplus
extern "C" {
#endif

#define def_panic_codes(_class,led,code) code,
typedef enum {
        NotUsedPanicCode = 0, // Not used
#include "panic_codes.h"
} ePanicCode;
#undef def_panic_codes

void panic_(ePanicCode code, void* extraInfo, void (*HAL_Delay_Microseconds)(uint32_t));

#ifdef __cplusplus
}
#endif

#endif /* PANIC_H_ */
