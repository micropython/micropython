/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "../../gfx.h"

#if GFX_USE_GMISC

#if GMISC_NEED_MATRIXFLOAT2D

#if !GMISC_NEED_FASTTRIG
	#include <math.h>
#endif

void gmiscMatrixFloat2DSetIdentity(MatrixFloat2D *m) {
	m->a00 = m->a11 = m->a22 = 1.0;
	m->a01 = m->a02 = m->a10 = m->a12 = m->a20 = m->a21 = 0.0;
}

void gmiscMatrixFloat2DMultiply(MatrixFloat2D *dst, const MatrixFloat2D *src1, const MatrixFloat2D *src2) {
	dst->a00 = src1->a00*src2->a00 + src1->a01*src2->a10 + src1->a02*src2->a20;
	dst->a01 = src1->a00*src2->a01 + src1->a01*src2->a11 + src1->a02*src2->a21;
	dst->a02 = src1->a00*src2->a02 + src1->a01*src2->a12 + src1->a02*src2->a22;
	dst->a10 = src1->a10*src2->a00 + src1->a11*src2->a10 + src1->a12*src2->a20;
	dst->a11 = src1->a10*src2->a01 + src1->a11*src2->a11 + src1->a12*src2->a21;
	dst->a12 = src1->a10*src2->a02 + src1->a11*src2->a12 + src1->a12*src2->a22;
	dst->a20 = src1->a20*src2->a00 + src1->a21*src2->a10 + src1->a22*src2->a20;
	dst->a21 = src1->a20*src2->a01 + src1->a21*src2->a11 + src1->a22*src2->a21;
	dst->a22 = src1->a20*src2->a02 + src1->a21*src2->a12 + src1->a22*src2->a22;
}

void gmiscMatrixFloat2DApplyTranslation(MatrixFloat2D *dst, const MatrixFloat2D *src, float tx, float ty) {
	if (src) {
		dst->a00 = src->a00; dst->a01 = src->a01; dst->a02 = src->a02+tx;
		dst->a10 = src->a10; dst->a11 = src->a11; dst->a12 = src->a12+ty;
		dst->a20 = src->a20; dst->a21 = src->a21; dst->a22 = src->a22;
	} else {
		dst->a00 = 1.0; dst->a01 = 0.0; dst->a02 = tx;
		dst->a10 = 0.0; dst->a11 = 1.0; dst->a12 = ty;
		dst->a20 = 0.0; dst->a21 = 0.0; dst->a22 = 1.0;
	}
}

void gmiscMatrixFloat2DApplyScale(MatrixFloat2D *dst, const MatrixFloat2D *src, float sx, float sy) {
	if (src) {
		dst->a00 = src->a00*sx; dst->a01 = src->a01*sy; dst->a02 = src->a02;
		dst->a10 = src->a10*sx; dst->a11 = src->a11*sy; dst->a12 = src->a12;
		dst->a20 = src->a20*sx; dst->a21 = src->a21*sy; dst->a22 = src->a22;
	} else {
		dst->a00 = sx;  dst->a01 = 0.0; dst->a02 = 0.0;
		dst->a10 = 0.0; dst->a11 = sy;  dst->a12 = 0.0;
		dst->a20 = 0.0; dst->a21 = 0.0; dst->a22 = 1.0;
	}
}

void gmiscMatrixFloat2DApplyShear(MatrixFloat2D *dst, const MatrixFloat2D *src, float sx, float sy) {
	if (src) {
		dst->a00 = src->a00 + src->a01*sy; dst->a01 = src->a00*sx + src->a01; dst->a02 = src->a02;
		dst->a10 = src->a10 + src->a11*sy; dst->a11 = src->a10*sx + src->a11; dst->a12 = src->a12;
		dst->a20 = src->a20 + src->a21*sy; dst->a21 = src->a20*sx + src->a21; dst->a22 = src->a22;
	} else {
		dst->a00 = 1.0; dst->a01 = sx;  dst->a02 = 0.0;
		dst->a10 = sy;  dst->a11 = 1.0; dst->a12 = 0.0;
		dst->a20 = 0.0; dst->a21 = 0.0; dst->a22 = 1.0;
	}
}

void gmiscMatrixFloat2DApplyRotation(MatrixFloat2D *dst, const MatrixFloat2D *src, int angle) {
	float	s, c;

	#if GMISC_NEED_FASTTRIG
		s = fsin(angle);
		c = fcos(angle);
	#else
		c = angle*GFX_PI/180;
		s = sin(c);
		c = cos(c);
	#endif

	if (src) {
		dst->a00 = src->a00*c - src->a01*s;	dst->a01 = src->a00*s + src->a01*c;	dst->a02 = src->a02;
		dst->a10 = src->a10*c - src->a11*s;	dst->a11 = src->a10*s + src->a11*c;	dst->a12 = src->a12;
		dst->a20 = src->a20*c - src->a21*s;	dst->a21 = src->a20*s + src->a21*c;	dst->a22 = src->a22;
	} else {
		dst->a00 = c;   dst->a01 = s;   dst->a02 = 0.0;
		dst->a10 = -s;  dst->a11 = c;   dst->a12 = 0.0;
		dst->a20 = 0.0; dst->a21 = 0.0; dst->a22 = 1.0;
	}
}

void gmiscMatrixFloat2DApplyToPoints(point *dst, const point *src, const MatrixFloat2D *m, int cnt) {
	float	x;

	for( ;cnt--; dst++, src++) {
		x = src->x;						// This allows in-place operation
		dst->x = round(x*m->a00+src->y*m->a01+m->a02);
		dst->y = round(x*m->a10+src->y*m->a11+m->a12);
	}
}

#endif // GMISC_NEED_MATRIXFLOAT2D

#if GMISC_NEED_MATRIXFIXED2D

void gmiscMatrixFixed2DSetIdentity(MatrixFixed2D *m) {
	m->a00 = m->a11 = m->a22 = FIXED(1);
	m->a01 = m->a02 = m->a10 = m->a12 = m->a20 = m->a21 = 0;
}

void gmiscMatrixFixed2DMultiply(MatrixFixed2D *dst, const MatrixFixed2D *src1, const MatrixFixed2D *src2) {
	dst->a00 = FIXEDMUL(src1->a00,src2->a00) + FIXEDMUL(src1->a01,src2->a10) + FIXEDMUL(src1->a02,src2->a20);
	dst->a01 = FIXEDMUL(src1->a00,src2->a01) + FIXEDMUL(src1->a01,src2->a11) + FIXEDMUL(src1->a02,src2->a21);
	dst->a02 = FIXEDMUL(src1->a00,src2->a02) + FIXEDMUL(src1->a01,src2->a12) + FIXEDMUL(src1->a02,src2->a22);
	dst->a10 = FIXEDMUL(src1->a10,src2->a00) + FIXEDMUL(src1->a11,src2->a10) + FIXEDMUL(src1->a12,src2->a20);
	dst->a11 = FIXEDMUL(src1->a10,src2->a01) + FIXEDMUL(src1->a11,src2->a11) + FIXEDMUL(src1->a12,src2->a21);
	dst->a12 = FIXEDMUL(src1->a10,src2->a02) + FIXEDMUL(src1->a11,src2->a12) + FIXEDMUL(src1->a12,src2->a22);
	dst->a20 = FIXEDMUL(src1->a20,src2->a00) + FIXEDMUL(src1->a21,src2->a10) + FIXEDMUL(src1->a22,src2->a20);
	dst->a21 = FIXEDMUL(src1->a20,src2->a01) + FIXEDMUL(src1->a21,src2->a11) + FIXEDMUL(src1->a22,src2->a21);
	dst->a22 = FIXEDMUL(src1->a20,src2->a02) + FIXEDMUL(src1->a21,src2->a12) + FIXEDMUL(src1->a22,src2->a22);
}

void gmiscMatrixFixed2DApplyTranslation(MatrixFixed2D *dst, const MatrixFixed2D *src, fixed tx, fixed ty) {
	if (src) {
		dst->a00 = src->a00; dst->a01 = src->a01; dst->a02 = src->a02+tx;
		dst->a10 = src->a10; dst->a11 = src->a11; dst->a12 = src->a12+ty;
		dst->a20 = src->a20; dst->a21 = src->a21; dst->a22 = src->a22;
	} else {
		dst->a00 = FIXED(1); dst->a01 = 0; dst->a02 = tx;
		dst->a10 = 0; dst->a11 = FIXED(1); dst->a12 = ty;
		dst->a20 = 0; dst->a21 = 0; dst->a22 = FIXED(1);
	}
}

void gmiscMatrixFixed2DApplyScale(MatrixFixed2D *dst, const MatrixFixed2D *src, fixed sx, fixed sy) {
	if (src) {
		dst->a00 = FIXEDMUL(sx,src->a00); dst->a01 = FIXEDMUL(sy,src->a01); dst->a02 = src->a02;
		dst->a10 = FIXEDMUL(sx,src->a10); dst->a11 = FIXEDMUL(sy,src->a11); dst->a12 = src->a12;
		dst->a20 = FIXEDMUL(sx,src->a20); dst->a21 = FIXEDMUL(sy,src->a21); dst->a22 = src->a22;
	} else {
		dst->a00 = sx; dst->a01 = 0;  dst->a02 = 0;
		dst->a10 = 0;  dst->a11 = sy; dst->a12 = 0;
		dst->a20 = 0;  dst->a21 = 0;  dst->a22 = FIXED(1);
	}
}

void gmiscMatrixFixed2DApplyShear(MatrixFixed2D *dst, const MatrixFixed2D *src, fixed sx, fixed sy) {
	if (src) {
		dst->a00 = src->a00 + FIXEDMUL(sy,src->a01); dst->a01 = FIXEDMUL(sx,src->a00) + src->a01; dst->a02 = src->a02;
		dst->a10 = src->a10 + FIXEDMUL(sy,src->a11); dst->a11 = FIXEDMUL(sx,src->a10) + src->a11; dst->a12 = src->a12;
		dst->a20 = src->a20 + FIXEDMUL(sy,src->a21); dst->a21 = FIXEDMUL(sx,src->a20) + src->a21; dst->a22 = src->a22;
	} else {
		dst->a00 = FIXED(1); dst->a01 = sx; dst->a02 = 0;
		dst->a10 = sy; dst->a11 = FIXED(1); dst->a12 = 0;
		dst->a20 = 0; dst->a21 = 0; dst->a22 = FIXED(1);
	}
}

#if GMISC_NEED_FIXEDTRIG
	void gmiscMatrixFixed2DApplyRotation(MatrixFixed2D *dst, const MatrixFixed2D *src, int angle) {
		fixed	s, c;

		s = ffsin(angle);
		c = ffcos(angle);

		if (src) {
			dst->a00 = FIXEDMUL(c,src->a00) - FIXEDMUL(s,src->a01);	dst->a01 = FIXEDMUL(s,src->a00) + FIXEDMUL(c,src->a01);	dst->a02 = src->a02;
			dst->a10 = FIXEDMUL(c,src->a10) - FIXEDMUL(s,src->a11);	dst->a11 = FIXEDMUL(s,src->a10) + FIXEDMUL(c,src->a11);	dst->a12 = src->a12;
			dst->a20 = FIXEDMUL(c,src->a20) - FIXEDMUL(s,src->a21);	dst->a21 = FIXEDMUL(s,src->a20) + FIXEDMUL(c,src->a21);	dst->a22 = src->a22;
		} else {
			dst->a00 = c;  dst->a01 = s; dst->a02 = 0;
			dst->a10 = -s; dst->a11 = c; dst->a12 = 0;
			dst->a20 = 0;  dst->a21 = 0; dst->a22 = FIXED(1);
		}
	}
#endif

void gmiscMatrixFixed2DApplyToPoints(point *dst, const point *src, const MatrixFixed2D *m, int cnt) {
	coord_t	x;

	for( ;cnt--; dst++, src++) {
		x = src->x;						// This allows in-place operation
		dst->x = NONFIXED(x*m->a00+src->y*m->a01+m->a02+FIXED0_5);
		dst->y = NONFIXED(x*m->a10+src->y*m->a11+m->a12+FIXED0_5);
	}
}

#endif // GMISC_NEED_MATRIXFIXED2D

#endif // GFX_USE_GMISC
