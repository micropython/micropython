#include <stdint.h>
#include <string.h>

#include "mpconfigport.h"
#include "mpgpio.h"

uint8_t gpio_find_index(const char* name)
{
    uint8_t instance = 0;
    if(strcmp(name, "gpio_1") == 0)
    {
        instance = 1;
    }
    else if(strcmp(name, "gpio_2") == 0)
    {
        instance = 2;
    }
    else if(strcmp(name, "gpio_3") == 0)
    {
        instance = 3;
    }
    else if(strcmp(name, "gpio_4") == 0)
    {
        instance = 4;
    }
    else if(strcmp(name, "gpio_5") == 0)
    {
        instance = 5;
    }
    else if(strcmp(name, "gpio_6") == 0)
    {
        instance = 6;
    }
    else if(strcmp(name, "gpio_7") == 0)
    {
        instance = 7;
    }else if(strcmp(name, "gpio_8") == 0)
    {
        instance = 8;
    }
    else if(strcmp(name, "gpio_9") == 0)
    {
        instance = 9;
    }
    return instance;
}

