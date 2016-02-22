#ifndef __SIMPLE_CONFIG_H__
#define __SIMPLE_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif


  /*  This macro means user take simple config
   *  lib to another platform such as linux, and
   *  have no rom crypto libs of simple config,
   *  so we take simple_config_crypto as a sw lib 
   *  This macro is used by Realtek internal to generate simple config lib
   *  Please delete this macro after generation.
   */
#define SIMPLE_CONFIG_PLATFORM_LIB 0

#include "platform_opts.h"
#include "autoconf.h"



/* platform related settings */
#undef u32
#undef s32
#undef u8
#undef s8
#undef u16
#undef s16
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned char u8;
typedef char s8;
typedef unsigned short int u16;
typedef signed short int s16;

typedef s32 (*simple_config_printf_fn) (const s8 *fmt, ...);
typedef void* (*simple_config_memset_fn) (void *dst0, s32 Val, u32 length);
typedef void* (*simple_config_memcpy_fn) ( void *s1, const void *s2, u32 n );
typedef u32 (*simple_config_strlen_fn) (const char *s);
typedef char * (*simple_config_strcpy_fn) (s8 *dest, const s8 *src);
typedef void (*simple_config_free_fn) (u8 *pbuf, u32 sz);
typedef u8*  (*simple_config_zmalloc_fn) (u32 sz);
typedef u8* (*simple_config_malloc_fn) (u32 sz);
typedef int (*simple_config_memcmp_fn) (const void *av, const void *bv, u32 len);
typedef u32 (*simple_config_ntohl_fn)(u32 x);



struct simple_config_lib_config {
	simple_config_printf_fn printf;
	simple_config_memset_fn memset;
	simple_config_memcpy_fn memcpy;
	simple_config_strlen_fn strlen;
	simple_config_strcpy_fn strcpy;
	simple_config_free_fn free;
	simple_config_zmalloc_fn zmalloc;
	simple_config_malloc_fn malloc;
	simple_config_memcmp_fn memcmp;
	simple_config_ntohl_fn _ntohl;


	int *is_promisc_callback_unlock;

};


/* expose data */
extern s32 is_promisc_callback_unlock;
extern u8 g_bssid[6];
extern u8 g_security_mode;

/* expose API */
extern s32 rtk_sc_init(char *custom_pin_code, struct simple_config_lib_config* config);
extern s32 rtk_start_parse_packet(u8 *da, u8 *sa, s32 len,  void * user_data, void *backup_sc);
extern void rtk_restart_simple_config(void);
extern void rtk_sc_deinit();
extern void wifi_enter_promisc_mode();


#ifdef __cplusplus
}
#endif

#endif /* __SIMPLE_CONFIG_H__*/
