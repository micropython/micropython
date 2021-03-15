
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

#define BOOT_RUNNING BLUE
#define MAIN_RUNNING GREEN
#define SAFE_MODE    YELLOW
#define ALL_DONE     GREEN
#define REPL_RUNNING WHITE

#define ACTIVE_WRITE 0x200000

#define ALL_GOOD_CYCLE_MS 2000u

#define LINE_NUMBER_TOGGLE_LENGTH 300u
#define EXCEPTION_TYPE_LENGTH_MS 1000u

#define THOUSANDS WHITE
#define HUNDREDS  BLUE
#define TENS      YELLOW
#define ONES      CYAN

#define INDENTATION_ERROR GREEN
#define SYNTAX_ERROR      CYAN
#define NAME_ERROR        WHITE
#define OS_ERROR          ORANGE
#define VALUE_ERROR       PURPLE
#define MPY_ERROR         BLUE
#define OTHER_ERROR       YELLOW
