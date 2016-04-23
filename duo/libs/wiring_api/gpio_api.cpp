#include "spark_wiring.h"

#ifdef __cplusplus
extern "C" {
#endif


void wiring_analogWrite(uint16_t pin, uint16_t value)
{
	analogWrite(pin, value);
}

void wiring_analogWriteWithFreq(uint16_t pin, uint16_t value, uint16_t pwm_frequency)
{
	analogWrite(pin, value, pwm_frequency);
}


#ifdef __cplusplus
}
#endif
