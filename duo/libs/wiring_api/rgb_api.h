#ifndef _RGB_API_H
#define _RGB_API_H


bool rgb_controlled(void);
void rgb_controll(bool override);
void rgb_color(int red, int green, int blue);
void rgb_setBrightness(uint8_t brightness, bool update);;


#endif
