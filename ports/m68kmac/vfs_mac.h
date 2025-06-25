#pragma once

typedef struct _mp_obj_vfs_mac_t mp_obj_vfs_mac_t;
typedef struct _mp_obj_vfs_mac_file_t mp_obj_vfs_mac_file_t;

extern const mp_obj_type_t mp_type_vfs_mac;
extern const mp_obj_type_t mp_type_vfs_mac_fileio;
extern const mp_obj_type_t mp_type_vfs_mac_textio;

mp_obj_t mp_vfs_mac_file_open(const mp_obj_vfs_mac_t *fs, const mp_obj_type_t *type, mp_obj_t file_in, mp_obj_t mode_in);
