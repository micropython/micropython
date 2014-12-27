typedef struct _user_mount_t {
    const char *str;
    mp_uint_t len;
    mp_obj_t readblocks[4];
    mp_obj_t writeblocks[4];
    mp_obj_t sync[2];
    mp_obj_t count[2];
    FATFS fatfs;
} user_mount_t;

extern user_mount_t user_mount;
