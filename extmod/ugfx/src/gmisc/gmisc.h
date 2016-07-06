/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gmisc/gmisc.h
 * @brief   GMISC - Miscellaneous Routines header file.
 *
 * @addtogroup GMISC
 *
 * @brief	Module which contains different features such as array conversions
 *
 * @{
 */

#ifndef _GMISC_H
#define _GMISC_H

#include "../../gfx.h"

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

// Forward definition
typedef struct point point;

/**
 * @brief	Sample data formats
 * @note	These are defined regardless of whether you use the GMISC module
 * 			or not as they are used in lots of places.
 */
typedef enum ArrayDataFormat_e {
	ARRAY_DATA_UNKNOWN = 0,
	ARRAY_DATA_4BITUNSIGNED = 4,	ARRAY_DATA_4BITSIGNED = 5,
	ARRAY_DATA_8BITUNSIGNED = 8,	ARRAY_DATA_8BITSIGNED = 9,
	ARRAY_DATA_10BITUNSIGNED = 10,	ARRAY_DATA_10BITSIGNED = 11,
	ARRAY_DATA_12BITUNSIGNED = 12,	ARRAY_DATA_12BITSIGNED = 13,
	ARRAY_DATA_14BITUNSIGNED = 14,	ARRAY_DATA_14BITSIGNED = 15,
	ARRAY_DATA_16BITUNSIGNED = 16,	ARRAY_DATA_16BITSIGNED = 17,
	} ArrayDataFormat;

/**
 * @brief	Is the sample data format a "signed" data format?
 */
#define gfxSampleFormatIsSigned(fmt)	((fmt) & 1)

/**
 * @brief	How many bits are in the sample data format
 */
#define gfxSampleFormatBits(fmt)	((fmt) & ~1)

/**
 * @brief   The type for a fixed point type.
 * @details	The top 16 bits are the integer component, the bottom 16 bits are the real component.
 */
typedef int32_t	fixed;

/**
 * @brief   Macros to convert to and from a fixed point.
 * @{
 */
#define FIXED(x)			((fixed)(x)<<16)						/* @< integer to fixed */
#define NONFIXED(x)			((x)>>16)								/* @< fixed to integer */
#define FIXED0_5			32768									/* @< 0.5 as a fixed (used for rounding) */
#define FP2FIXED(x)			((fixed)((x)*65536.0))					/* @< floating point to fixed */
#define FIXED2FP(x)			((double)(x)/65536.0)					/* @< fixed to floating point */
#define FIXEDMUL(a,b)		((fixed)((((long long)(a))*(b))>>16))	/* @< fixed,fixed multiplication */
#define FIXEDMULINT(a,b)	((a)*(b))								/* @< integer,fixed multiplication */
/** @} */

/**
 * @brief   The famous number pi
 */
#ifndef GFX_PI
	#define GFX_PI	3.1415926535897932384626433832795028841971693993751
#endif

/**
 * @brief   pi as a fixed point
 */
#define FIXED_PI	FP2FIXED(GFX_PI)

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#if GFX_USE_GMISC || defined(__DOXYGEN__)

#ifdef __cplusplus
extern "C" {
#endif

#if GMISC_NEED_ARRAYOPS || defined(__DOXYGEN__)
	/**
	 * @brief				Convert from one array format to another array format.
	 * @pre					Requires GFX_USE_GMISC and GMISC_NEED_ARRAYOPS
	 *
	 * @param[in] srcfmt		The format of the source array
	 * @param[in] src			The source array
	 * @param[in] dstfmt		The format of the destination array
	 * @param[in] dst			The dstination array
	 * @param[in] cnt			The number of array elements to convert
	 *
	 * @note				Assumes the destination buffer is large enough for the resultant data.
	 * @note				This routine is optimised to perform as fast as possible.
	 * @note				No type checking is performed on the source format. It is assumed to
	 * 						have only valid values eg. ARRAY_DATA_4BITSIGNED will have values
	 * 							0000 -> 0111 for positive numbers and 1111 -> 1000 for negative numbers
	 * 							Bits 5 -> 8 in the storage byte are treated in an undefined manner.
	 * @note				If srcfmt or dstfmt is an unknown format, this routine does nothing
	 * 						with no warning that something is wrong
	 *
	 * @api
	 */
	void gmiscArrayConvert(ArrayDataFormat srcfmt, void *src, ArrayDataFormat dstfmt, void *dst, size_t cnt);

	#if 0
		void gmiscArrayTranslate(ArrayDataFormat fmt, void *src, void *dst, size_t cnt, int trans);

		void gmiscArrayMultiply(ArrayDataFormat fmt, void *src, void *dst, size_t cnt, int mult);

		void gmiscArrayDivide(ArrayDataFormat fmt, void *src, void *dst, size_t cnt, int mdiv);

		void gmiscArrayMultDiv(ArrayDataFormat fmt, void *src, void *dst, size_t cnt, int mult, int div);

		void gmiscArrayAdd(ArrayDataFormat fmt, void *src1, void *src2, void *dst, size_t cnt);

		void gmiscArrayAddNoOverflow(ArrayDataFormat fmt, void *src1, void *src2, void *dst, size_t cnt);
	#endif
#endif

#if GMISC_NEED_FASTTRIG || defined(__DOXYGEN__)
		extern const double sintabledouble[];

		/**
		 * @brief	Fast Table Based Trig functions
		 * @return	A double in the range -1.0 .. 0.0 .. 1.0
		 * @pre		Requires GFX_USE_GMISC and GMISC_NEED_FASTTRIG
		 *
		 * @param[in] degrees	The angle in degrees (not radians)
		 *
		 * @note	These functions use degrees rather than radians to describe the angle.
		 *
		 * @api
		 * @{
		 */
		double fsin(int degrees);
		double fcos(int degrees);
		/** @}
		 *
		 * @brief	Fast Table Based Trig functions
		 * @return	A double in the range -1.0 .. 0.0 .. 1.0
		 * @pre		Requires GFX_USE_GMISC and GMISC_NEED_FASTTRIG
		 *
		 * @param[in] degrees	The angle in degrees 0 .. 359
		 *
		 * @note	These functions use degrees rather than radians to describe the angle.
		 * @note	These functions are super fast but require the parameter to be in range.
		 * 			Use the lowercase functions if the parameter may not be in range or if a
		 * 			required trig function is not supported in this form.
		 *
		 * @api
		 * @{
		 */
		#define FSIN(degrees) 	sintabledouble[degrees];
		/** @} */
#endif

#if GMISC_NEED_FIXEDTRIG || defined(__DOXYGEN__)
		extern const fixed sintablefixed[];

		/**
		 * @brief	Fast Table Based Trig functions
		 * @return	A fixed point in the range -1.0 .. 0.0 .. 1.0
		 * @pre		Requires GFX_USE_GMISC and GMISC_NEED_FIXEDTRIG
		 *
		 * @param[in] degrees	The angle in degrees (not radians)
		 *
		 * @note	These functions use degrees rather than radians to describe the angle.
		 *
		 * @api
		 * @{
		 */
		fixed ffsin(int degrees);
		fixed ffcos(int degrees);
		/** @}
		 *
		 * @brief	Fast Table Based Trig functions
		 * @return	A fixed point in the range -1.0 .. 0.0 .. 1.0
		 * @pre		Requires GFX_USE_GMISC and GMISC_NEED_FIXEDTRIG
		 *
		 * @param[in] degrees	The angle in degrees 0 .. 359
		 *
		 * @note	These functions use degrees rather than radians to describe the angle.
		 * @note	These functions are super fast but require the parameter to be in range.
		 * 			Use the lowercase functions if the parameter may not be in range or if a
		 * 			required trig function is not supported in this form.
		 *
		 * @api
		 * @{
		 */
		#define FFSIN(degrees) 	sintablefixed[degrees];
		/** @} */
#endif

#if GMISC_NEED_INVSQRT || defined(__DOXYGEN__)
		/**
		 * @brief	Fast inverse square root function (x^-1/2)
		 * @return	The approximate inverse square root
		 * @pre		Requires GFX_USE_GMISC and GMISC_NEED_INVSQRT
		 *
		 * @param[in] n	The number to find the inverse square root of
		 *
		 * @note	This function generates an approximate result. Higher accuracy (at the expense
		 * 			of speed) can be obtained by modifying the source code (the necessary line
		 * 			is already there - just commented out).
		 * @note	This function relies on the internal machine format of a float and a long.
		 * 			If your machine architecture is very unusual this function may not work.
		 *
		 * @api
		 */
		float invsqrt(float n);
#endif

#if GMISC_NEED_MATRIXFLOAT2D || defined(__DOXYGEN__)

	/**
	 * @brief	A matrix for doing 2D graphics using floats
	 * @pre		Requires GFX_USE_GMISC and GMISC_NEED_MATRIXFLOAT2D
	 */
	typedef struct MatrixFloat2D {
		float	a00, a01, a02;
		float	a10, a11, a12;
		float	a20, a21, a22;
	} MatrixFloat2D;

	/**
	 * @brief	Apply the matrix to a set of points
	 * @pre		Requires GFX_USE_GMISC and GMISC_NEED_MATRIXFLOAT2D
	 *
	 * @param[in] dst	The destination array of points
	 * @param[in] src	The source array of points
	 * @param[in] m		The matrix to apply
	 * @param[in] cnt	How many points are in the array
	 *
	 * @note	In-place matrix application is allowed ie. dst = src
	 *
	 * @api
	 */
	void gmiscMatrixFloat2DApplyToPoints(point *dst, const point *src, const MatrixFloat2D *m, int cnt);

	/**
	 * @brief	Set the 2D matrix to the identity matrix
	 * @pre		Requires GFX_USE_GMISC and GMISC_NEED_MATRIXFLOAT2D
	 *
	 * @param[in] m		The matrix to set to identity
	 *
	 * @api
	 */
	void gmiscMatrixFloat2DSetIdentity(MatrixFloat2D *m);

	/**
	 * @brief	Multiple two 2D matrixes together
	 * @pre		Requires GFX_USE_GMISC and GMISC_NEED_MATRIXFLOAT2D
	 *
	 * @param[in] dst	The destination matrix
	 * @param[in] src1	The first source matrix
	 * @param[in] src2	The second source matrix
	 *
	 * @note	In-place matrix application is NOT allowed ie. dst != src1, dst != src2
	 *
	 * @api
	 */
	void gmiscMatrixFloat2DMultiply(MatrixFloat2D *dst, const MatrixFloat2D *src1, const MatrixFloat2D *src2);

	/**
	 * @brief	Add an x,y translation to a matrix
	 * @pre		Requires GFX_USE_GMISC and GMISC_NEED_MATRIXFLOAT2D
	 *
	 * @param[in] dst		The destination matrix
	 * @param[in] src		The source matrix. Can be NULL
	 * @param[in] tx, ty	The x and y translation to apply
	 *
	 * @note	In-place matrix operation is NOT allowed ie. dst != src
	 * @note	If no source matrix is provided, it is equivalent to applying the operation
	 * 			to an identity matrix. It also is a much simpler operation requiring no multiplication.
	 *
	 * @api
	 */
	void gmiscMatrixFloat2DApplyTranslation(MatrixFloat2D *dst, const MatrixFloat2D *src, float tx, float ty);

	/**
	 * @brief	Add x,y scaling to a matrix
	 * @pre		Requires GFX_USE_GMISC and GMISC_NEED_MATRIXFLOAT2D
	 *
	 * @param[in] dst		The destination matrix
	 * @param[in] src		The source matrix. Can be NULL
	 * @param[in] sx, sy	The scaling to apply in the x and y direction. Negative numbers give reflection.
	 *
	 * @note	In-place matrix operation is NOT allowed ie. dst != src
	 * @note	If no source matrix is provided, it is equivalent to applying the operation
	 * 			to an identity matrix. It also is a much simpler operation requiring no multiplication.
	 *
	 * @api
	 */
	void gmiscMatrixFloat2DApplyScale(MatrixFloat2D *dst, const MatrixFloat2D *src, float sx, float sy);

	/**
	 * @brief	Add x,y shear to a matrix
	 * @pre		Requires GFX_USE_GMISC and GMISC_NEED_MATRIXFLOAT2D
	 *
	 * @param[in] dst		The destination matrix
	 * @param[in] src		The source matrix. Can be NULL
	 * @param[in] sx, sy	The shear to apply in the x and y direction.
	 *
	 * @note	In-place matrix operation is NOT allowed ie. dst != src
	 * @note	If no source matrix is provided, it is equivalent to applying the operation
	 * 			to an identity matrix. It also is a much simpler operation requiring no multiplication.
	 *
	 * @api
	 */
	void gmiscMatrixFloat2DApplyShear(MatrixFloat2D *dst, const MatrixFloat2D *src, float sx, float sy);

	/**
	 * @brief	Add rotation to a matrix
	 * @pre		Requires GFX_USE_GMISC and GMISC_NEED_MATRIXFLOAT2D
	 *
	 * @param[in] dst		The destination matrix
	 * @param[in] src		The source matrix. Can be NULL
	 * @param[in] angle		The angle to apply in degrees (not radians).
	 *
	 * @note	In-place matrix operation is NOT allowed ie. dst != src
	 * @note	If no source matrix is provided, it is equivalent to applying the operation
	 * 			to an identity matrix. It also is a much simpler operation.
	 * @note	If GMISC_NEED_FASTTRIG is defined then the fast table sin and cos lookup's will be used
	 * 			rather than the C library versions.
	 *
	 * @api
	 */
	void gmiscMatrixFloat2DApplyRotation(MatrixFloat2D *dst, const MatrixFloat2D *src, int angle);
#endif

#if GMISC_NEED_MATRIXFIXED2D || defined(__DOXYGEN__)

	/**
	 * @brief	A matrix for doing 2D graphics using fixed point maths
	 * @pre		Requires GFX_USE_GMISC and GMISC_NEED_MATRIXFIXED2D
	 */
	typedef struct MatrixFixed2D {
		fixed	a00, a01, a02;
		fixed	a10, a11, a12;
		fixed	a20, a21, a22;
	} MatrixFixed2D;

	/**
	 * @brief	Apply the matrix to a set of points
	 * @pre		Requires GFX_USE_GMISC and GMISC_NEED_MATRIXFIXED2D
	 *
	 * @param[in] dst	The destination array of points
	 * @param[in] src	The source array of points
	 * @param[in] m		The matrix to apply
	 * @param[in] cnt	How many points are in the array
	 *
	 * @note	In-place matrix application is allowed ie. dst = src
	 *
	 * @api
	 */
	void gmiscMatrixFixed2DApplyToPoints(point *dst, const point *src, const MatrixFixed2D *m, int cnt);

	/**
	 * @brief	Set the 2D matrix to the identity matrix
	 * @pre		Requires GFX_USE_GMISC and GMISC_NEED_MATRIXFIXED2D
	 *
	 * @param[in] m		The matrix to set to identity
	 *
	 * @api
	 */
	void gmiscMatrixFixed2DSetIdentity(MatrixFixed2D *m);

	/**
	 * @brief	Multiple two 2D matrixes together
	 * @pre		Requires GFX_USE_GMISC and GMISC_NEED_MATRIXFIXED2D
	 *
	 * @param[in] dst	The destination matrix
	 * @param[in] src1	The first source matrix
	 * @param[in] src2	The second source matrix
	 *
	 * @note	In-place matrix application is NOT allowed ie. dst != src1, dst != src2
	 *
	 * @api
	 */
	void gmiscMatrixFixed2DMultiply(MatrixFixed2D *dst, const MatrixFixed2D *src1, const MatrixFixed2D *src2);

	/**
	 * @brief	Add an x,y translation to a matrix
	 * @pre		Requires GFX_USE_GMISC and GMISC_NEED_MATRIXFIXED2D
	 *
	 * @param[in] dst		The destination matrix
	 * @param[in] src		The source matrix. Can be NULL
	 * @param[in] tx, ty	The x and y translation to apply
	 *
	 * @note	In-place matrix operation is NOT allowed ie. dst != src
	 * @note	If no source matrix is provided, it is equivalent to applying the operation
	 * 			to an identity matrix. It also is a much simpler operation requiring no multiplication.
	 *
	 * @api
	 */
	void gmiscMatrixFixed2DApplyTranslation(MatrixFixed2D *dst, const MatrixFixed2D *src, fixed tx, fixed ty);

	/**
	 * @brief	Add x,y scaling to a matrix
	 * @pre		Requires GFX_USE_GMISC and GMISC_NEED_MATRIXFIXED2D
	 *
	 * @param[in] dst		The destination matrix
	 * @param[in] src		The source matrix. Can be NULL
	 * @param[in] sx, sy	The scaling to apply in the x and y direction. Negative numbers give reflection.
	 *
	 * @note	In-place matrix operation is NOT allowed ie. dst != src
	 * @note	If no source matrix is provided, it is equivalent to applying the operation
	 * 			to an identity matrix. It also is a much simpler operation requiring no multiplication.
	 *
	 * @api
	 */
	void gmiscMatrixFixed2DApplyScale(MatrixFixed2D *dst, const MatrixFixed2D *src, fixed sx, fixed sy);

	/**
	 * @brief	Add x,y shear to a matrix
	 * @pre		Requires GFX_USE_GMISC and GMISC_NEED_MATRIXFIXED2D
	 *
	 * @param[in] dst		The destination matrix
	 * @param[in] src		The source matrix. Can be NULL
	 * @param[in] sx, sy	The shear to apply in the x and y direction.
	 *
	 * @note	In-place matrix operation is NOT allowed ie. dst != src
	 * @note	If no source matrix is provided, it is equivalent to applying the operation
	 * 			to an identity matrix. It also is a much simpler operation requiring no multiplication.
	 *
	 * @api
	 */
	void gmiscMatrixFixed2DApplyShear(MatrixFixed2D *dst, const MatrixFixed2D *src, fixed sx, fixed sy);

	#if GMISC_NEED_FIXEDTRIG || defined(__DOXYGEN__)
		/**
		 * @brief	Add rotation to a matrix
		 * @pre		Requires GFX_USE_GMISC and GMISC_NEED_MATRIXFIXED2D and GMISC_NEED_FIXEDTRIG
		 *
		 * @param[in] dst		The destination matrix
		 * @param[in] src		The source matrix. Can be NULL
		 * @param[in] angle		The angle to apply in degrees (not radians).
		 *
		 * @note	In-place matrix operation is NOT allowed ie. dst != src
		 * @note	If no source matrix is provided, it is equivalent to applying the operation
		 * 			to an identity matrix. It also is a much simpler operation requiring no multiplication.
		 *
		 * @api
		 */
		void gmiscMatrixFixed2DApplyRotation(MatrixFixed2D *dst, const MatrixFixed2D *src, int angle);
	#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_MISC */

#endif /* _GMISC_H */
/** @} */

