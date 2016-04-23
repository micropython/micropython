#include "spark_wiring_rgb.h"

#ifdef __cplusplus
extern "C" {
#endif


bool rgb_controlled(void)
{
    return RGB.controlled();
}

void rgb_controll(bool override)
{
    RGB.control(override);
}

void rgb_color(int red, int green, int blue)
{
    RGB.color(red, green, blue);
}

void rgb_setBrightness(uint8_t brightness, bool update)
{
    RGB.brightness(brightness, update);
}

uint8_t rgb_getBrightness()
{
    return RGB.brightness();
}


#ifdef __cplusplus
}
#endif
