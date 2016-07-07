#ifndef SSD1351_H
#define SSD1351_H

#define SSD1351_SET_COLUMN_ADDRESS              0x15
#define SSD1351_SET_ROW_ADDRESS                 0x75
#define SSD1351_WRITE_RAM                       0x5C
#define SSD1351_READ_RAM                        0x5D
#define SSD1351_SET_REMAP                       0xA0

#define SSD1351_SET_DISPLAY_START               0xA1
#define SSD1351_SET_DISPLAY_OFFSET              0xA2
#define SSD1351_SET_DISPLAY_MODE_ALL_OFF        0xA4
#define SSD1351_SET_DISPLAY_MODE_ALL_ON         0xA5
#define SSD1351_SET_DISPLAY_MODE_RESET          0xA6
#define SSD1351_SET_DISPLAY_MODE_INVERT         0xA7
#define SSD1351_SET_FUNCTION_SELECT             0xAB
#define SSD1351_SET_SLEEP_ON                    0xAE
#define SSD1351_SET_SLEEP_OFF                   0xAF
#define SSD1351_SET_RESET_PRECHARGE             0xB1

#define SSD1351_DISPLAY_ENHANCEMENT             0xB2
#define SSD1351_CLOCKDIV_OSCFREQ                0xB3
#define SSD1351_SET_VSL                         0xB4
#define SSD1351_SET_GPIO                        0xB5
#define SSD1351_SET_SECOND_PRECHARGE            0xB6

#define SSD1351_LUT_GRAYSCALE                   0xB8
#define SSD1351_USE_BUILTIN_LUT                 0xB9
#define SSD1351_SET_PRECHARGE                   0xBB
#define SSD1351_SET_VCOMH                       0xBE

#define SSD1351_SET_CONTRAST                    0xC1
#define SSD1351_MASTER_CONTRAST_CURRENT_CONTROL 0xC7
#define SSD1351_SET_MUX_RATIO                   0xCA
#define SSD1351_SET_COMMAND_LOCK                0xFD

#endif // SSD1351_H
