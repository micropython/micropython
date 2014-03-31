void mp_module_init(void);
void mp_module_deinit(void);
mp_obj_t mp_module_get(qstr module_name);
void mp_module_register(qstr qstr, mp_obj_t module);
