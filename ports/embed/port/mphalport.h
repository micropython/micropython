// Define so there's no dependency on extmod/virtpin.h
#define mp_hal_pin_obj_t

#if MICROPY_KBD_EXCEPTION
void mp_hal_set_interrupt_char(int c);
#endif
