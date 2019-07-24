/* ==========================================
    CMock Project - Automatic Mock Generation for C
    Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
    [Released under MIT License. Please refer to license.txt for details]
========================================== */

typedef unsigned short U16;
typedef signed int int32_t;

/* CMock should handle UTF-8 characters in comments. The world is an awesomely diverse place! */
/* my µC Rocks! Open Source, not ©! My language has no Ümlauts! ǺƜǝƧǾɱɛ! */ /**! Illegal: åäö */

typedef struct _POINT_T
{
  int x;
  int y;
} POINT_T;

// typedef edge case;
// not ANSI C but it has been done and will break cmock if not handled
typedef void VOID_TYPE_CRAZINESS;

/* fun parsing & mock generation cases */

void var_args1(int a, ...);
void var_args2(int a, int b, ...);

VOID_TYPE_CRAZINESS void_type_craziness1(int, float, double, char, short, long, long int, long long, void*);
int void_type_craziness2( VOID_TYPE_CRAZINESS );

   void  crazy_whitespace  (   int    lint, double shot  ,  short  stack )  ;

char
 crazy_multiline
(
  int a,
  unsigned int b);

U16  *ptr_return1(int a);
U16*  ptr_return2(int a);
U16 * ptr_return3(int a);

unsigned int** ptr_ptr_return1(unsigned int** a);
unsigned int* *ptr_ptr_return2(unsigned int* *a);
unsigned int **ptr_ptr_return3(unsigned int **a);
unsigned int ** ptr_ptr_return4(unsigned int ** a);

extern unsigned long int incredible_descriptors(register const unsigned short a);

int32_t example_c99_type(int32_t param1);

void I2CIntRegister(uint32_t ui32Base, void (*pfnHandler)(void));
