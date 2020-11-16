const mp_obj_type_t mp_type_EspError;

#define raise_esp_error(err) mp_raise_msg_varg(&mp_type_EspError, MP_ERROR_TEXT("%d(0x%X) - %s"), err, err, esp_err_to_name(err));

