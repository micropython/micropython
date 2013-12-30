#include <stdint.h>

#include "misc.h"

// attribute flags
#define FL_PRINT (0x01)
#define FL_SPACE (0x02)
#define FL_DIGIT (0x04)
#define FL_ALPHA (0x08)
#define FL_UPPER (0x10)
#define FL_LOWER (0x20)

// shorthand character attributes
#define AT_PR (FL_PRINT)
#define AT_SP (FL_SPACE | FL_PRINT)
#define AT_DI (FL_DIGIT | FL_PRINT)
#define AT_AL (FL_ALPHA | FL_PRINT)
#define AT_UP (FL_UPPER | FL_ALPHA | FL_PRINT)
#define AT_LO (FL_LOWER | FL_ALPHA | FL_PRINT)

// table of attributes for ascii characters
static const uint8_t attr[] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, AT_SP, AT_SP, AT_SP, 0, AT_SP, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    AT_SP, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR,
    AT_PR, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR,
    AT_DI, AT_DI, AT_DI, AT_DI, AT_DI, AT_DI, AT_DI, AT_DI,
    AT_DI, AT_DI, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR,
    AT_PR, AT_UP, AT_UP, AT_UP, AT_UP, AT_UP, AT_UP, AT_UP,
    AT_UP, AT_UP, AT_UP, AT_UP, AT_UP, AT_UP, AT_UP, AT_UP,
    AT_UP, AT_UP, AT_UP, AT_UP, AT_UP, AT_UP, AT_UP, AT_UP,
    AT_UP, AT_UP, AT_UP, AT_PR, AT_PR, AT_PR, AT_PR, AT_PR,
    AT_PR, AT_LO, AT_LO, AT_LO, AT_LO, AT_LO, AT_LO, AT_LO,
    AT_LO, AT_LO, AT_LO, AT_LO, AT_LO, AT_LO, AT_LO, AT_LO,
    AT_LO, AT_LO, AT_LO, AT_LO, AT_LO, AT_LO, AT_LO, AT_LO,
    AT_LO, AT_LO, AT_LO, AT_PR, AT_PR, AT_PR, AT_PR, 0
};

unichar utf8_get_char(const char *s) {
    return *s;
}

char *utf8_next_char(const char *s) {
    return (char*)(s + 1);
}

bool unichar_isspace(unichar c) {
    return c < 128 && (attr[c] & FL_SPACE) != 0;
}

bool unichar_isalpha(unichar c) {
    return c < 128 && (attr[c] & FL_ALPHA) != 0;
}

bool unichar_isprint(unichar c) {
    return c < 128 && (attr[c] & FL_PRINT) != 0;
}

bool unichar_isdigit(unichar c) {
    return c < 128 && (attr[c] & FL_DIGIT) != 0;
}

/*
bool char_is_alpha_or_digit(unichar c) {
    return c < 128 && (attr[c] & (FL_ALPHA | FL_DIGIT)) != 0;
}

bool char_is_upper(unichar c) {
    return c < 128 && (attr[c] & FL_UPPER) != 0;
}

bool char_is_lower(unichar c) {
    return c < 128 && (attr[c] & FL_LOWER) != 0;
}
*/
