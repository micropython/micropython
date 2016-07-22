#include <zephyr.h>
#include "zephyr_getchar.h"

int real_main(void);

void main(void) {
    zephyr_getchar_init();
    real_main();
}
