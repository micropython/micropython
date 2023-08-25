extern "C" {
#include <analogue_readings.hpp>

mp_obj_t trigger_readings() {
    // code to do readings
    // and write to EPROM
    return mp_const_none;
}

mp_obj_t read_batch(mp_obj_t batch_size, mp_obj_t offset) {
    int byte_size = mp_obj_get_int(batch_size);
	int byte_offset = mp_obj_get_int(offset);
    mp_obj_t list = mp_obj_new_list(size, NULL);

	// some function to read from EPROM
    // maybe a lambda called read_eprom

    for (int i = 0; i < batch_size; i++) {
        auto eprom_reading = read_eprom(offset + i);
        mp_obj_list_append(list, mp_obj_new_int(eprom_reading));
    }

    return list;
}

}
