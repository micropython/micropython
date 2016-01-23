#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "rtl8195a.h"

int main(void)
{
    int i=0;

    while (1) {
        DiagPrintf("Hello World : %d\r\n", i++);
        HalDelayUs(500000);
        DiagPrintf("This is test\r\n");
        DiagPrintf("This is a\r\n");
    }
}

