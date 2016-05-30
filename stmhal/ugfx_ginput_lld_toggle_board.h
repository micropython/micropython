#ifndef _GDISP_LLD_TOGGLE_BOARD_H
#define _GDISP_LLD_TOGGLE_BOARD_H

#error "GINPUT Toggle Pal Driver: You need to define your board definitions"

// The below are example values

#define GINPUT_TOGGLE_NUM_PORTS			8			// The total number of toggle inputs
#define GINPUT_TOGGLE_CONFIG_ENTRIES	8			// The total number of GToggleConfig entries

#define GINPUT_TOGGLE_RIGHT   0				// Joystick Right
#define GINPUT_TOGGLE_UP      1				// Joystick Up
#define GINPUT_TOGGLE_A       2				// Switch 1
#define GINPUT_TOGGLE_B       3				// Switch 2
#define GINPUT_TOGGLE_MENU    4
#define GINPUT_TOGGLE_DOWN    5				// Joystick Down
#define GINPUT_TOGGLE_LEFT    6				// Joystick Left
#define GINPUT_TOGGLE_CENTER  7				// Joystick Center

/*
PD3 - RIGHT
PD6 - UP
PD8 - A
PD9 - B
PD10 - MENU
PD11 - DOWN
PE0 - CENTRE
PA15 - LEFT
*/

#define GINPUT_TOGGLE_DECLARE_STRUCTURE()											\
const GToggleConfig GInputToggleConfigTable[GINPUT_TOGGLE_CONFIG_ENTRIES] = {   \
    {GPIOD,       \
     GPIO_PIN_3,  \
     GPIO_PIN_3,  \
     0},          \
    {GPIOD,       \
     GPIO_PIN_6,  \
     GPIO_PIN_6,  \
     0},          \
    {GPIOD,       \
     GGPIO_PIN_8, \
     GPIO_PIN_8,  \
     0},          \
    {GPIOD,       \
     GPIO_PIN_9,  \
     GPIO_PIN_9,  \
     0},          \
    {GPIOD,       \
     GPIO_PIN_10, \
     GPIO_PIN_10, \
     0},          \
    {GPIOD,       \
     GPIO_PIN_11, \
     GPIO_PIN_11, \
     0},          \
    {GPIOE,       \
     GPIO_PIN_0,  \
     GPIO_PIN_0,  \
     0},          \
    {GPIOA,       \
     GPIO_PIN_15, \
     GPIO_PIN_15, \
     0},          \
}

#endif /* _GDISP_LLD_TOGGLE_BOARD_H */