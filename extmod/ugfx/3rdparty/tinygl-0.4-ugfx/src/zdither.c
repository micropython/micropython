/* 
 * Highly optimised dithering 16 bits -> 8 bits. 
 * The formulas were taken in Mesa (Bob Mercier mercier@hollywood.cinenet.net).
 */

#include <stdlib.h>
#include <stdio.h>
#include "zbuffer.h"
#include <assert.h>

#if defined(TGL_FEATURE_8_BITS)

#define _R	5
#define _G	9
#define _B	5
#define _DX	4
#define _DY	4
#define _D	(_DX*_DY)
#define _MIX(r,g,b)	( ((g)<<6) | ((b)<<3) | (r) )

#define DITHER_TABLE_SIZE (1 << 15)

#define DITHER_INDEX(r,g,b) ((b) + (g) * _B + (r) * (_B * _G))

#define MAXC	256
static int kernel8[_DY*_DX] = {
    0 * MAXC,  8 * MAXC,  2 * MAXC, 10 * MAXC,
   12 * MAXC,  4 * MAXC, 14 * MAXC,  6 * MAXC,
    3 * MAXC, 11 * MAXC,  1 * MAXC,  9 * MAXC,
   15 * MAXC,  7 * MAXC, 13 * MAXC,  5 * MAXC,
};

/* we build the color table and the lookup table */

void ZB_initDither(ZBuffer *zb,int nb_colors,
		   unsigned char *color_indexes,int *color_table)
{
  int c,r,g,b,i,index,r1,g1,b1;

  if (nb_colors < (_R * _G * _B)) {
	gl_fatal_error("zdither: not enough colors\n");
  }

  for(i=0;i<nb_colors;i++) color_table[i]=0;

  zb->nb_colors=nb_colors;
  zb->ctable=gl_malloc(nb_colors * sizeof(int));

  for (r = 0; r < _R; r++) {
    for (g = 0; g < _G; g++) {
      for (b = 0; b < _B; b++) {
	r1=(r*255) / (_R - 1);
	g1=(g*255) / (_G - 1);
	b1=(b*255) / (_B - 1);
	index=DITHER_INDEX(r,g,b);
	c=(r1 << 16) | (g1 << 8) | b1;
	zb->ctable[index]=c;
	color_table[index]=c;
      }
    }
  }

  zb->dctable=gl_malloc( DITHER_TABLE_SIZE );

  for(i=0;i<DITHER_TABLE_SIZE;i++) {
    r=(i >> 12) & 0x7;
    g=(i >> 8) & 0xF;
    b=(i >> 3) & 0x7;
    index=DITHER_INDEX(r,g,b);
    zb->dctable[i]=color_indexes[index];
  }
}

void ZB_closeDither(ZBuffer *zb)
{
    gl_free(zb->ctable);
    gl_free(zb->dctable);
}

#if 0
int ZDither_lookupColor(int r,int g,int b)
{
  unsigned char *ctable=zdither_color_table;
  return ctable[_MIX(_DITH0(_R, r), _DITH0(_G, g),_DITH0(_B, b))];
}
#endif


#define DITHER_PIXEL2(a)			\
{ \
  register int v,t,r,g,c;			\
  v=*(unsigned int *)(pp+(a));                  \
  g=(v & 0x07DF07DF) + g_d; \
  r=(((v & 0xF800F800) >> 2) + r_d) & 0x70007000; \
  t=r | g; \
  c=ctable[t & 0xFFFF] | (ctable[t >> 16] << 8); \
  *(unsigned short *)(dest+(a))=c; 	\
}

/* NOTE: all the memory access are 16 bit aligned, so if buf or
   linesize are not multiple of 2, it cannot work efficiently (or
   hang!) */

void ZB_ditherFrameBuffer(ZBuffer *zb,unsigned char *buf,
			  int linesize)
{
  int xk,yk,x,y,c1,c2;
  unsigned char *dest1;
  unsigned short *pp1;
  int r_d,g_d,b_d;
  unsigned char *ctable=zb->dctable;
  register unsigned char *dest;
  register unsigned short *pp;

  assert( ((long)buf & 1) == 0 && (linesize & 1) == 0);

  for(yk=0;yk<4;yk++) {
    for(xk=0;xk<4;xk+=2) {
#if BYTE_ORDER == BIG_ENDIAN
      c1=kernel8[yk*4+xk+1];
      c2=kernel8[yk*4+xk];
#else
      c1=kernel8[yk*4+xk];
      c2=kernel8[yk*4+xk+1];
#endif
      r_d=((c1 << 2) & 0xF800) >> 2;
      g_d=(c1 >> 4) & 0x07C0;
      b_d=(c1 >> 9) & 0x001F;
      
      r_d|=(((c2 << 2) & 0xF800) >> 2) << 16;
      g_d|=((c2 >> 4) & 0x07C0) << 16;
      b_d|=((c2 >> 9) & 0x001F) << 16;
      g_d=b_d | g_d;

      dest1=buf + (yk * linesize) + xk;
      pp1=zb->pbuf + (yk * zb->xsize) + xk;
      
      for(y=yk;y<zb->ysize;y+=4) {
	dest=dest1;
	pp=pp1;
	for(x=xk;x<zb->xsize;x+=16) {

	  DITHER_PIXEL2(0);
	  DITHER_PIXEL2(1*4);
	  DITHER_PIXEL2(2*4);
	  DITHER_PIXEL2(3*4);

	  pp+=16;
	  dest+=16;
	}
	dest1+=linesize*4;
	pp1+=zb->xsize*4;
      }
    }
  }
}

#endif
