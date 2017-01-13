#define BLACK   0x000000
#define GREEN   0x003f00
#define BLUE    0x00003f
#define CYAN    0x003f3f
#define RED     0x3f0000
#define ORANGE  0x3f1f00
#define YELLOW  0x3f3f00
#define PURPLE  0x3f003f
#define WHITE   0x3f3f3f

#define BOOT_RUNNING BLUE
#define MAIN_RUNNING GREEN
#define ALL_DONE     GREEN
#define REPL_RUNNING WHITE

#define ACTIVE_WRITE 0x8f0000

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
#define OTHER_ERROR       YELLOW
