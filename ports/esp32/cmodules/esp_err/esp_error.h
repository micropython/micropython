const mp_obj_type_t esp_err_EspError_type;

#define raise_esp_error(err) mp_raise_msg_varg(&esp_err_EspError_type, MP_ERROR_TEXT("%d(0x%X) - "#err), err, err);
 