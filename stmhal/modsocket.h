typedef struct {
    mp_obj_base_t base;
    int fd;
} socket_t;
extern const mp_obj_type_t socket_type;
void modsocket_init0();
