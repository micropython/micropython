#ifndef __MICROPY_INCLUDED_MICROBIT_IMAGE_H__
#define __MICROPY_INCLUDED_MICROBIT_IMAGE_H__

#include "py/runtime.h"

#define MAX_BRIGHTNESS 9

/** Monochrome images are immutable, which means that
 * we only need one bit per pixel which saves quite a lot
 * of memory */

/* we reserve a couple of bits, so we won't need to modify the
 * layout if we need to add more functionality or subtypes. */
#define TYPE_AND_FLAGS \
    mp_obj_base_t base; \
    uint8_t five:1; \
    uint8_t reserved1:1; \
    uint8_t reserved2:1

typedef struct _image_base_t {
    TYPE_AND_FLAGS;
} image_base_t;

typedef struct _monochrome_5by5_t {
    TYPE_AND_FLAGS;
    uint8_t pixel44: 1;
    uint8_t bits24[3];
} monochrome_5by5_t;


/* This is an internal method it is up to the caller to validate the inputs */
uint8_t monochromeGetPixelValue(monochrome_5by5_t * p_mono, mp_int_t x, mp_int_t y);

typedef struct _greyscale_t {
    TYPE_AND_FLAGS;
    uint8_t height;
    uint8_t width;
    uint8_t byte_data[]; /* Static initializer for this will have to be C, not C++ */
} greyscale_t;

#if 1
void clear(greyscale_t * p_greyscale);
/* Thiese are internal methods and it is up to the caller to validate the inputs */
uint8_t greyscaleGetPixelValue(greyscale_t * p_greyscale, mp_int_t x, mp_int_t y);
void greyscaleSetPixelValue(greyscale_t * p_greyscale, mp_int_t x, mp_int_t y, mp_int_t val);
void greyscaleFill(greyscale_t * p_greyscale, mp_int_t val);
#endif

typedef union _microbit_image_obj_t {
    image_base_t base;
    monochrome_5by5_t monochrome_5by5;
    greyscale_t greyscale;
} microbit_image_obj_t;

#if 1
mp_int_t imageHeight(microbit_image_obj_t * p_image_obj);
mp_int_t imageWidth(microbit_image_obj_t * p_image_obj);
greyscale_t * imageCopy(microbit_image_obj_t * p_image_obj);
greyscale_t * imageInvert(microbit_image_obj_t * p_image_obj);
/* This is an internal method it is up to the caller to validate the inputs */
uint8_t imageGetPixelValue(microbit_image_obj_t * p_image_obj, mp_int_t x, mp_int_t y);
#endif

/** Return a facade object that presents the string as a sequence of images */
mp_obj_t microbit_string_facade(mp_obj_t string);

void microbit_image_set_from_char(greyscale_t *img, char c);
microbit_image_obj_t *microbit_image_for_char(char c);
mp_obj_t microbit_image_slice(microbit_image_obj_t *img, mp_int_t start, mp_int_t width, mp_int_t stride);
/* ref exists so that we can pull a string out of an object and not have it GC'ed while oterating over it */
mp_obj_t scrolling_string_image_iterable(const char* str, mp_uint_t len, mp_obj_t ref, bool monospace, bool repeat);

#define SMALL_IMAGE(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p44) \
{ \
    { &microbit_image_type }, \
    1, 0, 0, (p44), \
    { \
        (p0)|((p1)<<1)|((p2)<<2)|((p3)<<3)|((p4)<<4)|((p5)<<5)|((p6)<<6)|((p7)<<7), \
        (p8)|((p9)<<1)|((p10)<<2)|((p11)<<3)|((p12)<<4)|((p13)<<5)|((p14)<<6)|((p15)<<7), \
        (p16)|((p17)<<1)|((p18)<<2)|((p19)<<3)|((p20)<<4)|((p21)<<5)|((p22)<<6)|((p23)<<7) \
    } \
}

extern const monochrome_5by5_t microbit_blank_image;
extern const monochrome_5by5_t microbit_const_image_heart_obj;
extern const mp_obj_type_t microbit_image_type;

#define BLANK_IMAGE (microbit_image_obj_t *)(&microbit_blank_image)
#define HEART_IMAGE (microbit_image_obj_t *)(&microbit_const_image_heart_obj)
#define HAPPY_IMAGE (microbit_image_obj_t *)(&microbit_const_image_happy_obj)

#if MICROPY_PY_BUILTINS_FLOAT
microbit_image_obj_t *microbit_image_dim(microbit_image_obj_t *lhs, mp_float_t fval);
#else
microbit_image_obj_t *microbit_image_dim(microbit_image_obj_t *lhs, mp_int_t val);
#endif
microbit_image_obj_t *microbit_image_sum(microbit_image_obj_t *lhs, microbit_image_obj_t *rhs, bool add);

#endif // __MICROPY_INCLUDED_MICROBIT_IMAGE_H__
