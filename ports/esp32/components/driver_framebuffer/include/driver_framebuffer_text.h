#pragma once

#include <stdint.h>
#include <stdbool.h>

/* Fonts */

#ifdef __cplusplus
extern "C" {
#endif

extern const GFXfont fairlight_8pt7b;
extern const GFXfont fairlight_12pt7b;
extern const GFXfont org_01_8pt7b;
extern const GFXfont dejavusans_20pt7b;
extern const GFXfont permanentmarker_22pt7b;
extern const GFXfont permanentmarker_36pt7b;
extern const GFXfont roboto_Black_22pt7b;
extern const GFXfont roboto_BlackItalic_24pt7b;
extern const GFXfont roboto_12pt7b;
extern const GFXfont roboto_18pt7b;
extern const GFXfont roboto_22pt7b;
extern const GFXfont weather_42pt8b;
extern const GFXfont ocra_16pt7b;
extern const GFXfont ocra_22pt7b;
extern const GFXfont Exo2_Regular_22pt7b;
extern const GFXfont Exo2_Thin_22pt7b;
extern const GFXfont Exo2_Bold_22pt7b;
extern const GFXfont Exo2_Regular_18pt7b;
extern const GFXfont Exo2_Thin_18pt7b;
extern const GFXfont Exo2_Bold_18pt7b;
extern const GFXfont Exo2_Regular_12pt7b;
extern const GFXfont Exo2_Thin_12pt7b;
extern const GFXfont Exo2_Bold_12pt7b;
extern const GFXfont ipane7x5;
extern const GFXfont PressStart2P_6pt7b;
extern const GFXfont PressStart2P_8pt7b;
extern const GFXfont PressStart2P_9pt7b;
extern const GFXfont PressStart2P_12pt7b;
extern const GFXfont PressStart2P_18pt7b;
extern const GFXfont PressStart2P_22pt7b;

/* Functions */

const GFXfont* driver_framebuffer_findFontByName(const char* fontName);
uint16_t driver_framebuffer_print(Window* window, const char* str, int16_t x0, int16_t y0, uint8_t xScale, uint8_t yScale, uint32_t color, const GFXfont *font);
uint16_t driver_framebuffer_print_len(Window* window, const char* str, int16_t len, int16_t x0, int16_t y0, uint8_t xScale, uint8_t yScale, uint32_t color, const GFXfont *font);
uint16_t driver_framebuffer_get_string_width(const char* str, const GFXfont *font);
uint16_t driver_framebuffer_get_string_height(const char* str, const GFXfont *font);

#ifdef __cplusplus
}
#endif
