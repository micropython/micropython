extern "C" {
#include <analogue_readings.hpp>

#define MAX_READINGS 16384
STATIC int cnt = 0;

mp_obj_t make_analogue_readings() {
    mp_obj_t list = mp_obj_new_list(0, NULL);

    for (int i = 0; i < MAX_READINGS; i++) {
        mp_obj_list_append(list, mp_obj_new_int(i));
        cnt += 1;
    }

    return list;
}

}