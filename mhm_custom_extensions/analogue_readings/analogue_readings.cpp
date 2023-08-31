#include <iostream>

extern "C"
{
#include "analouge_readings.h"

    mp_obj_t trigger_readings()
    {
        std::cout << "Trigger readings stub\n";
        return mp_const_none;
    }

    mp_obj_t read_batch(mp_obj_t batch_size, mp_obj_t offset)
    {
        std::cout << "Read batch stub\n";
        mp_obj_t list = mp_obj_new_list(0, NULL);
        return list;
    }
}
