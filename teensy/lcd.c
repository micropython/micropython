#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "parse.h"
#include "obj.h"
#include "../stmhal/lcd.h"

void lcd_init(void) {
}

void lcd_print_str(const char *str) {
    (void)str;
}

void lcd_print_strn(const char *str, unsigned int len) {
    (void)str;
    (void)len;
}
