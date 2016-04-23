#ifndef DEVICE_NAME_H
#define	DEVICE_NAME_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_LOCAL_NAME_SIZE 20

typedef struct
{
    uint8_t length;
    uint8_t value[ MAX_LOCAL_NAME_SIZE ];
} local_name_t;

void HAL_Local_Name(local_name_t *local_name);

#ifdef __cplusplus
}
#endif

#endif
