#ifndef __THREADING_ALT_H__
#define __THREADING_ALT_H__


#include "FreeRTOS.h"
#include "semphr.h"

typedef struct
{
    SemaphoreHandle_t mutex;
    char is_valid;
} mbedtls_threading_mutex_t;

extern void mbedtls_threading_set_alt( void ( * mutex_init )( mbedtls_threading_mutex_t * ),
                                       void ( * mutex_free )( mbedtls_threading_mutex_t * ),
                                       int ( * mutex_lock )( mbedtls_threading_mutex_t * ),
                                       int ( * mutex_unlock )( mbedtls_threading_mutex_t * ) );


#endif /* ifndef __THREADING_ALT_H__ */
