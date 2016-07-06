{
    int n, dx, dy, sx, pp_inc_1, pp_inc_2;
    register int a;
    register PIXEL *pp;
#if defined(INTERP_RGB) || TGL_FEATURE_RENDER_BITS == 24
    register unsigned int r, g, b;
#endif
#ifdef INTERP_RGB
    register unsigned int rinc, ginc, binc;
#endif
#ifdef INTERP_Z
    register unsigned short *pz;
    int zinc;
    register int z, zz;
#endif

    if (p1->y > p2->y || (p1->y == p2->y && p1->x > p2->x)) {
	ZBufferPoint *tmp;
	tmp = p1;
	p1 = p2;
	p2 = tmp;
    }
    sx = zb->xsize;
    pp = (PIXEL *) ((char *) zb->pbuf + zb->linesize * p1->y + p1->x * PSZB);
#ifdef INTERP_Z
    pz = zb->zbuf + (p1->y * sx + p1->x);
    z = p1->z;
#endif

    dx = p2->x - p1->x;
    dy = p2->y - p1->y;
#ifdef INTERP_RGB
    r = p2->r << 8;
    g = p2->g << 8;
    b = p2->b << 8;
#elif TGL_FEATURE_RENDER_BITS == 24
    /* for 24 bits, we store the colors in different variables */
    r = p2->r >> 8;
    g = p2->g >> 8;
    b = p2->b >> 8;
#endif

#undef RGB
#ifdef INTERP_RGB
#define RGB(x) x
#if TGL_FEATURE_RENDER_BITS == 24
#define RGBPIXEL pp[0] = r >> 16, pp[1] = g >> 16, pp[2] = b >> 16
#else
#define RGBPIXEL *pp = RGB_TO_PIXEL(r >> 8,g >> 8,b >> 8)
#endif
#else /* INTERP_RGB */
#define RGB(x)
#if TGL_FEATURE_RENDER_BITS == 24
#define RGBPIXEL pp[0] = r, pp[1] = g, pp[2] = b
#else
#define RGBPIXEL *pp = color
#endif
#endif /* INTERP_RGB */

#ifdef INTERP_Z
#define ZZ(x) x
#define PUTPIXEL() 				\
  {						\
    zz=z >> ZB_POINT_Z_FRAC_BITS;		\
    if (ZCMP(zz,*pz))  { 			\
      RGBPIXEL;	\
      *pz=zz; 					\
    }						\
  }
#else /* INTERP_Z */
#define ZZ(x)
#define PUTPIXEL() RGBPIXEL
#endif /* INTERP_Z */

#define DRAWLINE(dx,dy,inc_1,inc_2) \
    n=dx;\
    ZZ(zinc=(p2->z-p1->z)/n);\
    RGB(rinc=((p2->r-p1->r) << 8)/n;\
        ginc=((p2->g-p1->g) << 8)/n;\
        binc=((p2->b-p1->b) << 8)/n);\
    a=2*dy-dx;\
    dy=2*dy;\
    dx=2*dx-dy;\
    pp_inc_1 = (inc_1) * PSZB;\
    pp_inc_2 = (inc_2) * PSZB;\
    do {\
        PUTPIXEL();\
        ZZ(z+=zinc);\
        RGB(r+=rinc;g+=ginc;b+=binc);\
        if (a>0) { pp=(PIXEL *)((char *)pp + pp_inc_1); ZZ(pz+=(inc_1));  a-=dx; }\
	else { pp=(PIXEL *)((char *)pp + pp_inc_2); ZZ(pz+=(inc_2)); a+=dy; }\
    } while (--n >= 0);

/* fin macro */

    if (dx == 0 && dy == 0) {
	PUTPIXEL();
    } else if (dx > 0) {
	if (dx >= dy) {
	    DRAWLINE(dx, dy, sx + 1, 1);
	} else {
	    DRAWLINE(dy, dx, sx + 1, sx);
	}
    } else {
	dx = -dx;
	if (dx >= dy) {
	    DRAWLINE(dx, dy, sx - 1, -1);
	} else {
	    DRAWLINE(dy, dx, sx - 1, sx);
	}
    }
}

#undef INTERP_Z
#undef INTERP_RGB

/* internal defines */
#undef DRAWLINE
#undef PUTPIXEL
#undef ZZ
#undef RGB
#undef RGBPIXEL 
