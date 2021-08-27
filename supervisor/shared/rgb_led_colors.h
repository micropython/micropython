
#define COLOR(r, g, b) (((r) << 16) | ((g) << 8) | (b))
// For brightness == 255 (full). This will be adjusted downward for various different RGB indicators,
// which vary in brightness.
#define INTENSITY (0x30)

#define BLACK   COLOR(0, 0, 0)
#define GREEN   COLOR(0, INTENSITY, 0)
#define BLUE    COLOR(0, 0, INTENSITY)
#define CYAN    COLOR(0, INTENSITY, INTENSITY)
#define RED     COLOR(INTENSITY, 0, 0)
#define ORANGE  COLOR(INTENSITY, INTENSITY * 2 / 3, 0)
#define YELLOW  COLOR(INTENSITY, INTENSITY, 0)
#define PURPLE  COLOR(INTENSITY, 0, INTENSITY)
#define WHITE   COLOR(INTENSITY, INTENSITY, INTENSITY)

#define ALL_DONE     GREEN
#define EXCEPTION    RED
#define SAFE_MODE    YELLOW
#define REPL_RUNNING WHITE

#define ALL_DONE_BLINKS 1
#define EXCEPTION_BLINKS 2
#define SAFE_MODE_BLINKS 3

#define OFF_ON_RATIO 3

#define LED_SLEEP_TIME_MS 5000u

#define BLINK_TIME_MS 100u
