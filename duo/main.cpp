
#include "application.h"
#include "wiring.h"

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup()
{
}

void loop()
{
	mp_setup();
	mp_loop();
}


