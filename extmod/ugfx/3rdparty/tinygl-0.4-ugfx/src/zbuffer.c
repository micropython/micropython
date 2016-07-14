/*

 * Z buffer: 16 bits Z / 16 bits color
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "zbuffer.h"

ZBuffer *ZB_open(int xsize, int ysize, int mode,
		 int nb_colors,
		 unsigned char *color_indexes,
		 int *color_table,
		 void *frame_buffer)
{
    ZBuffer *zb;
    int size;

    zb = gl_malloc(sizeof(ZBuffer));
    if (zb == NULL)
	return NULL;

    zb->xsize = xsize;
    zb->ysize = ysize;
    zb->mode = mode;
    zb->linesize = (xsize * PSZB + 3) & ~3;

    switch (mode) {
#ifdef TGL_FEATURE_8_BITS
    case ZB_MODE_INDEX:
	ZB_initDither(zb, nb_colors, color_indexes, color_table);
	break;
#endif
#ifdef TGL_FEATURE_32_BITS
    case ZB_MODE_RGBA:
#endif
#ifdef TGL_FEATURE_24_BITS
    case ZB_MODE_RGB24:
#endif
    case ZB_MODE_5R6G5B:
	zb->nb_colors = 0;
	break;
    default:
	goto error;
    }

    size = zb->xsize * zb->ysize * sizeof(unsigned short);

    zb->zbuf = gl_malloc(size);
    if (zb->zbuf == NULL)
	goto error;

    if (frame_buffer == NULL) {
	zb->pbuf = gl_malloc(zb->ysize * zb->linesize);
	if (zb->pbuf == NULL) {
	    gl_free(zb->zbuf);
	    goto error;
	}
	zb->frame_buffer_allocated = 1;
    } else {
	zb->frame_buffer_allocated = 0;
	zb->pbuf = frame_buffer;
    }

    zb->current_texture = NULL;

    return zb;
  error:
    gl_free(zb);
    return NULL;
}

void ZB_close(ZBuffer * zb)
{
#ifdef TGL_FEATURE_8_BITS
    if (zb->mode == ZB_MODE_INDEX)
	ZB_closeDither(zb);
#endif

    if (zb->frame_buffer_allocated)
	gl_free(zb->pbuf);

    gl_free(zb->zbuf);
    gl_free(zb);
}

void ZB_resize(ZBuffer * zb, void *frame_buffer, int xsize, int ysize)
{
    int size;

    /* xsize must be a multiple of 4 */
    xsize = xsize & ~3;

    zb->xsize = xsize;
    zb->ysize = ysize;
    zb->linesize = (xsize * PSZB + 3) & ~3;

    size = zb->xsize * zb->ysize * sizeof(unsigned short);

    gl_free(zb->zbuf);
    zb->zbuf = gl_malloc(size);

    if (zb->frame_buffer_allocated)
	gl_free(zb->pbuf);

    if (frame_buffer == NULL) {
	zb->pbuf = gl_malloc(zb->ysize * zb->linesize);
	zb->frame_buffer_allocated = 1;
    } else {
	zb->pbuf = frame_buffer;
	zb->frame_buffer_allocated = 0;
    }
}

static void ZB_copyBuffer(ZBuffer * zb,
                          void *buf,
                          int linesize)
{
    unsigned char *p1;
    PIXEL *q;
    int y, n;

    q = zb->pbuf;
    p1 = buf;
    n = zb->xsize * PSZB;
    for (y = 0; y < zb->ysize; y++) {
	memcpy(p1, q, n);
	p1 += linesize;
	q = (PIXEL *) ((char *) q + zb->linesize);
    }
}

#if TGL_FEATURE_RENDER_BITS == 16

/* 32 bpp copy */

#ifdef TGL_FEATURE_32_BITS

#define RGB16_TO_RGB32(p0,p1,v)\
{\
    unsigned int g,b,gb;\
    g = (v & 0x07E007E0) << 5;\
    b = (v & 0x001F001F) << 3;\
    gb = g | b;\
    p0 = (gb & 0x0000FFFF) | ((v & 0x0000F800) << 8);\
    p1 = (gb >> 16) | ((v & 0xF8000000) >> 8);\
}

static void ZB_copyFrameBufferRGB32(ZBuffer * zb,
                                    void *buf,
                                    int linesize)
{
    unsigned short *q;
    unsigned int *p, *p1, v, w0, w1;
    int y, n;

    q = zb->pbuf;
    p1 = (unsigned int *) buf;

    for (y = 0; y < zb->ysize; y++) {
	p = p1;
	n = zb->xsize >> 2;
	do {
	    v = *(unsigned int *) q;
#if BYTE_ORDER == BIG_ENDIAN
	    RGB16_TO_RGB32(w1, w0, v);
#else
	    RGB16_TO_RGB32(w0, w1, v);
#endif
	    p[0] = w0;
	    p[1] = w1;

	    v = *(unsigned int *) (q + 2);
#if BYTE_ORDER == BIG_ENDIAN
	    RGB16_TO_RGB32(w1, w0, v);
#else
	    RGB16_TO_RGB32(w0, w1, v);
#endif
	    p[2] = w0;
	    p[3] = w1;

	    q += 4;
	    p += 4;
	} while (--n > 0);

	p1 += linesize;
    }
}

#endif

/* 24 bit packed pixel handling */

#ifdef TGL_FEATURE_24_BITS

/* order: RGBR GBRG BRGB */

/* XXX: packed pixel 24 bit support not tested */
/* XXX: big endian case not optimised */

#if BYTE_ORDER == BIG_ENDIAN

#define RGB16_TO_RGB24(p0,p1,p2,v1,v2)\
{\
    unsigned int r1,g1,b1,gb1,g2,b2,gb2;\
    v1 = (v1 << 16) | (v1 >> 16);\
    v2 = (v2 << 16) | (v2 >> 16);\
    r1 = (v1 & 0xF800F800);\
    g1 = (v1 & 0x07E007E0) << 5;\
    b1 = (v1 & 0x001F001F) << 3;\
    gb1 = g1 | b1;\
    p0 = ((gb1 & 0x0000FFFF) << 8) | (r1 << 16) | (r1 >> 24);\
    g2 = (v2 & 0x07E007E0) << 5;\
    b2 = (v2 & 0x001F001F) << 3;\
    gb2 = g2 | b2;\
    p1 = (gb1 & 0xFFFF0000) | (v2 & 0xF800) | ((gb2 >> 8) & 0xff);\
    p2 = (gb2 << 24) | ((v2 & 0xF8000000) >> 8) | (gb2 >> 16);\
}

#else

#define RGB16_TO_RGB24(p0,p1,p2,v1,v2)\
{\
    unsigned int r1,g1,b1,gb1,g2,b2,gb2;\
    r1 = (v1 & 0xF800F800);\
    g1 = (v1 & 0x07E007E0) << 5;\
    b1 = (v1 & 0x001F001F) << 3;\
    gb1 = g1 | b1;\
    p0 = ((gb1 & 0x0000FFFF) << 8) | (r1 << 16) | (r1 >> 24);\
    g2 = (v2 & 0x07E007E0) << 5;\
    b2 = (v2 & 0x001F001F) << 3;\
    gb2 = g2 | b2;\
    p1 = (gb1 & 0xFFFF0000) | (v2 & 0xF800) | ((gb2 >> 8) & 0xff);\
    p2 = (gb2 << 24) | ((v2 & 0xF8000000) >> 8) | (gb2 >> 16);\
}

#endif

static void ZB_copyFrameBufferRGB24(ZBuffer * zb,
                                    void *buf,
                                    int linesize)
{
    unsigned short *q;
    unsigned int *p, *p1, w0, w1, w2, v0, v1;
    int y, n;

    q = zb->pbuf;
    p1 = (unsigned int *) buf;
    linesize = linesize * 3;

    for (y = 0; y < zb->ysize; y++) {
	p = p1;
	n = zb->xsize >> 2;
	do {
	    v0 = *(unsigned int *) q;
	    v1 = *(unsigned int *) (q + 2);
	    RGB16_TO_RGB24(w0, w1, w2, v0, v1);
	    p[0] = w0;
	    p[1] = w1;
	    p[2] = w2;

	    q += 4;
	    p += 3;
	} while (--n > 0);

	p1 = (unsigned int *)(((char *)p1) + linesize);
    }
}

#endif

void ZB_copyFrameBuffer(ZBuffer * zb, void *buf,
			int linesize)
{
    switch (zb->mode) {
#ifdef TGL_FEATURE_8_BITS
    case ZB_MODE_INDEX:
	ZB_ditherFrameBuffer(zb, buf, linesize >> 1);
	break;
#endif
#ifdef TGL_FEATURE_16_BITS
    case ZB_MODE_5R6G5B:
	ZB_copyBuffer(zb, buf, linesize);
	break;
#endif
#ifdef TGL_FEATURE_32_BITS
    case ZB_MODE_RGBA:
	ZB_copyFrameBufferRGB32(zb, buf, linesize >> 1);
	break;
#endif
#ifdef TGL_FEATURE_24_BITS
    case ZB_MODE_RGB24:
	ZB_copyFrameBufferRGB24(zb, buf, linesize >> 1);
	break;
#endif
    default:
	gl_assert(0);
    }
}

#endif /* TGL_FEATURE_RENDER_BITS == 16 */

#if TGL_FEATURE_RENDER_BITS == 24

#define RGB24_TO_RGB16(r, g, b) \
  ((((r) >> 3) << 11) | (((g) >> 2) << 5) | ((b) >> 3))

/* XXX: not optimized */
static void ZB_copyFrameBuffer5R6G5B(ZBuffer * zb, 
                                     void *buf, int linesize) 
{
    PIXEL *q;
    unsigned short *p, *p1;
    int y, n;

    q = zb->pbuf;
    p1 = (unsigned short *) buf;

    for (y = 0; y < zb->ysize; y++) {
	p = p1;
	n = zb->xsize >> 2;
	do {
            p[0] = RGB24_TO_RGB16(q[0], q[1], q[2]);
            p[1] = RGB24_TO_RGB16(q[3], q[4], q[5]);
            p[2] = RGB24_TO_RGB16(q[6], q[7], q[8]);
            p[3] = RGB24_TO_RGB16(q[9], q[10], q[11]);
	    q = (PIXEL *)((char *)q + 4 * PSZB);
	    p += 4;
	} while (--n > 0);
	p1 = (unsigned short *)((char *)p1 + linesize);
    }
}

void ZB_copyFrameBuffer(ZBuffer * zb, void *buf,
			int linesize)
{
    switch (zb->mode) {
#ifdef TGL_FEATURE_16_BITS
    case ZB_MODE_5R6G5B:
	ZB_copyFrameBuffer5R6G5B(zb, buf, linesize);
	break;
#endif
#ifdef TGL_FEATURE_24_BITS
    case ZB_MODE_RGB24:
	ZB_copyBuffer(zb, buf, linesize);
	break;
#endif
    default:
	gl_assert(0);
    }
}

#endif /* TGL_FEATURE_RENDER_BITS == 24 */

#if TGL_FEATURE_RENDER_BITS == 32

#define RGB32_TO_RGB16(v) \
  (((v >> 8) & 0xf800) | (((v) >> 5) & 0x07e0) | (((v) & 0xff) >> 3))

/* XXX: not optimized */
static void ZB_copyFrameBuffer5R6G5B(ZBuffer * zb, 
                                     void *buf, int linesize) 
{
    PIXEL *q;
    unsigned short *p, *p1;
    int y, n;

    q = zb->pbuf;
    p1 = (unsigned short *) buf;

    for (y = 0; y < zb->ysize; y++) {
	p = p1;
	n = zb->xsize >> 2;
	do {
            p[0] = RGB32_TO_RGB16(q[0]);
            p[1] = RGB32_TO_RGB16(q[1]);
            p[2] = RGB32_TO_RGB16(q[2]);
            p[3] = RGB32_TO_RGB16(q[3]);
	    q += 4;
	    p += 4;
	} while (--n > 0);
	p1 = (unsigned short *)((char *)p1 + linesize);
    }
}

void ZB_copyFrameBuffer(ZBuffer * zb, void *buf,
			int linesize)
{
    switch (zb->mode) {
#ifdef TGL_FEATURE_16_BITS
    case ZB_MODE_5R6G5B:
	ZB_copyFrameBuffer5R6G5B(zb, buf, linesize);
	break;
#endif
#ifdef TGL_FEATURE_32_BITS
    case ZB_MODE_RGBA:
	ZB_copyBuffer(zb, buf, linesize);
	break;
#endif
    default:
	gl_assert(0);
    }
}

#endif /* TGL_FEATURE_RENDER_BITS == 32 */


/*
 * adr must be aligned on an 'int'
 */
void memset_s(void *adr, int val, int count)
{
    int i, n, v;
    unsigned int *p;
    unsigned short *q;

    p = adr;
    v = val | (val << 16);

    n = count >> 3;
    for (i = 0; i < n; i++) {
	p[0] = v;
	p[1] = v;
	p[2] = v;
	p[3] = v;
	p += 4;
    }

    q = (unsigned short *) p;
    n = count & 7;
    for (i = 0; i < n; i++)
	*q++ = val;
}

void memset_l(void *adr, int val, int count)
{
    int i, n, v;
    unsigned int *p;

    p = adr;
    v = val;
    n = count >> 2;
    for (i = 0; i < n; i++) {
	p[0] = v;
	p[1] = v;
	p[2] = v;
	p[3] = v;
	p += 4;
    }

    n = count & 3;
    for (i = 0; i < n; i++)
	*p++ = val;
}

/* count must be a multiple of 4 and >= 4 */
void memset_RGB24(void *adr,int r, int v, int b,long count)
{
    long i, n;
    register long v1,v2,v3,*pt=(long *)(adr);
    unsigned char *p,R=(unsigned char)r,V=(unsigned char)v,B=(unsigned char)b;

    p=(unsigned char *)adr;
    *p++=R;
    *p++=V;
    *p++=B;
    *p++=R;
    *p++=V;
    *p++=B;
    *p++=R;
    *p++=V;
    *p++=B;
    *p++=R;
    *p++=V;
    *p++=B;
    v1=*pt++;
    v2=*pt++;
    v3=*pt++;
    n = count >> 2;
    for(i=1;i<n;i++) {
        *pt++=v1;
        *pt++=v2;
        *pt++=v3;
    }
}

void ZB_clear(ZBuffer * zb, int clear_z, int z,
	      int clear_color, int r, int g, int b)
{
#if TGL_FEATURE_RENDER_BITS != 24
    int color;
#endif
    int y;
    PIXEL *pp;

    if (clear_z) {
	memset_s(zb->zbuf, z, zb->xsize * zb->ysize);
    }
    if (clear_color) {
	pp = zb->pbuf;
	for (y = 0; y < zb->ysize; y++) {
#if TGL_FEATURE_RENDER_BITS == 15 || TGL_FEATURE_RENDER_BITS == 16
            color = RGB_TO_PIXEL(r, g, b);
	    memset_s(pp, color, zb->xsize);
#elif TGL_FEATURE_RENDER_BITS == 32
            color = RGB_TO_PIXEL(r, g, b);
	    memset_l(pp, color, zb->xsize);
#elif TGL_FEATURE_RENDER_BITS == 24 
            memset_RGB24(pp,r>>8,g>>8,b>>8,zb->xsize);
#else
#error TODO
#endif
	    pp = (PIXEL *) ((char *) pp + zb->linesize);
	}
    }
}
