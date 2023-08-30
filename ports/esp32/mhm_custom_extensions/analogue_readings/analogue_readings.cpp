#include <iostream>

extern "C"
{
#include "analouge_readings.h"
#include "esp_partition.h"

    mp_obj_t trigger_readings()
    {
        std::cout << "hello world"
                  << "\n";
        return mp_const_none;
    }

    mp_obj_t read_batch(mp_obj_t batch_size, mp_obj_t offset)
    {
        int batch_size_num = mp_obj_get_int(batch_size);
        int byte_offset = mp_obj_get_int(offset);
        mp_obj_t list = mp_obj_new_list(0, NULL);

        const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, "storage");
        assert(partition != NULL);

        std::cout << "partition: " << partition->label << "\n";

        // some function to read from EPROM
        // maybe a lambda called read_eprom

        for (int i = 0; i < batch_size_num; i++)
        {
            // auto eprom_reading = read_eprom(offset + i);
            mp_obj_list_append(list, mp_obj_new_int(i));
        }

        return list;
    }
}
