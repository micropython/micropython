// instance object
// creating an instance of a class makes one of these objects
typedef struct _mp_obj_instance_t {
    mp_obj_base_t base;
    mp_map_t members;
    mp_obj_t subobj[];
    // TODO maybe cache __getattr__ and __setattr__ for efficient lookup of them
} mp_obj_instance_t;
