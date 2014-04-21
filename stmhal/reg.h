struct reg_s;
typedef struct reg_s reg_t;

typedef struct {
    mp_obj_base_t base;
    qstr base_name;
    uint32_t base_addr;
    const reg_t *reg;
    uint32_t num_regs;
} reg_base_obj_t;

typedef struct {
    mp_obj_base_t base;
} reg_map_obj_t;

extern const reg_map_obj_t reg_module;

