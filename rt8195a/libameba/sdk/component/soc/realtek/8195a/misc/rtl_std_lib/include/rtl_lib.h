/*
 * rtl_lib.h
 *
 * Definitions for RTL library functions
 */
 
#ifndef _RTL_LIB_H_
#define	_RTL_LIB_H_


#include <basic_types.h>
#include <diag.h>


extern int __rtl_errno;


void init_rom_libgloss_ram_map(void);


//
// RTL library functions for Libc::stdio
// 

extern int rtl_printf(IN const char* fmt, ...);
extern int rtl_sprintf(char* str, const char* fmt, ...);
extern int rtl_snprintf(char* str, size_t size, const char* fmt, ...);

//
// RTL library functions for string
// 

extern void * rtl_memchr(const void * src_void , int c , size_t length);
extern int rtl_memcmp(const void * m1 , const void * m2 , size_t n);
extern void * rtl_memcpy(void * dst0 , const void * src0 , size_t len0);
extern void * rtl_memmove( void * dst_void , const void * src_void , size_t length);
extern void * rtl_memset(void * m , int c , size_t n);
extern char * rtl_strcat(char * s1 , const char * s2);
extern char * rtl_strchr(const char *s1 , int i);
extern int rtl_strcmp(const char *s1 ,	const char *s2);
extern char* rtl_strcpy(char *dst0 , const char *src0);
extern size_t rtl_strlen(const char *str);
extern char * rtl_strncat(char * s1 , const char * s2 , size_t n);
extern int rtl_strncmp(const char *s1 , const char *s2 , size_t n);
extern char * rtl_strncpy(char * dst0 , const char * src0 , size_t count);
extern char * rtl_strstr(const char *searchee , const char *lookfor);
extern char * rtl_strsep(char **source_ptr , const char *delim);
extern char * rtl_strtok(char * s , const char * delim);

//
// RTL library functions for math
// 


extern double rtl_fabs(double);
extern float rtl_fabsf(float a);
extern float rtl_cos_f32(float a);
extern float rtl_sin_f32(float a);

extern float rtl_fadd(float a, float b);
extern float rtl_fsub(float a, float b);
extern float rtl_fmul(float a, float b);
extern float rtl_fdiv(float a, float b);

extern int rtl_fcmplt(float a, float b);
extern int rtl_fcmpgt(float a, float b);





//
// RTL eabi functions 

extern double rtl_ftod(float f);

extern double rtl_ddiv(double a, double b);


//
// Macro Library Functions
//

typedef union
{
  float value;
  u32 	word;
} ieee_float_shape_type;

/* Get a 32 bit int from a float.  */

#define GET_FLOAT_WORD(i,d)					\
do {								\
  ieee_float_shape_type gf_u;					\
  gf_u.value = (d);						\
  (i) = gf_u.word;						\
} while (0)

/* Set a float from a 32 bit int.  */

#define SET_FLOAT_WORD(d,i)					\
do {								\
  ieee_float_shape_type sf_u;					\
  sf_u.word = (i);						\
  (d) = sf_u.value;						\
} while (0)

static inline
float rtl_nanf(void)
{
	float x;

	SET_FLOAT_WORD(x,0x7fc00000);
	return x;
}


//
// Library Test functions
//

extern int rtl_lib_test(IN u16 argc, IN u8 *argv[]);
extern int rtl_math_test(IN u16 argc, IN u8 *argv[]);
extern int rtl_string_test(IN u16 argc, IN u8 *argv[]);


//
// Macro functions
//

#undef dbg_printf
#define dbg_printf(fmt, args...) \
	     	rtl_printf("%s():%d : " fmt "\n", __FUNCTION__, __LINE__, ##args); 


#undef err_printf
#define err_printf(fmt, args...) \
	     	rtl_printf("%s():%d : " fmt "\n", __FUNCTION__, __LINE__, ##args); 


#endif /* _RTL_LIB_H_ */
