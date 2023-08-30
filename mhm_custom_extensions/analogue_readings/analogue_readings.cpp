extern "C" {
#include <analogue_readings.hpp>

mp_obj_t trigger_readings() {
    // code to do readings
    // and write to EPROM
    return mp_const_none;
}

mp_obj_t read_batch(mp_obj_t batch_size, mp_obj_t offset) {
    mp_obj_t list = mp_obj_new_list(10, NULL);

    return list;
}

}
