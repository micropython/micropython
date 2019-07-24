/* ==========================================
    CMock Project - Automatic Mock Generation for C
    Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
    [Released under MIT License. Please refer to license.txt for details]
========================================== */

#if defined(__GNUC__) && !defined(__ICC)
#if !defined(__clang__)
#pragma GCC diagnostic ignored "-Wpragmas"
#endif
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wduplicate-decl-specifier"
#endif

struct _DUMMY_T { unsigned int a; float b; };

void const_variants1( const char* a, int const, unsigned short const * c );

void const_variants2(
	struct _DUMMY_T const * const param1,
	const unsigned long int const * const param2,
	const struct _DUMMY_T const * param3 );

const int * const_retval1(void);        /* nicety version for pointer to constant int */
int const * const_retval2(void);        /* formal version for pointer to constant int */
//int * const const_retval3(void);        /* formal version for constant pointer to int */
//int const * const const_retval4(void);  /* formal version for constant pointer to constant int */

const int* const_retval5(void);         /* sticky-left nicety version for pointer to constant int */
int const* const_retval6(void);         /* sticky-left formal version for pointer to constant int */
//int* const const_retval7(void);         /* sticky-left formal version for constant pointer to int */
//int const* const const_retval8(void);   /* sticky-left formal version for constant pointer to constant int */

const int *const_retval9(void);         /* sticky-right nicety version for pointer to constant int */
int const *const_retvalA(void);         /* sticky-right formal version for pointer to constant int */
//int *const const_retvalB(void);         /* sticky-right formal version for constant pointer to int */
//int const *const const_retvalC(void);   /* sticky-right formal version for constant pointer to constant int */
