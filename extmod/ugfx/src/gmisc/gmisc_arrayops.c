/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "../../gfx.h"

#if GFX_USE_GMISC && GMISC_NEED_ARRAYOPS

void gmiscArrayConvert(ArrayDataFormat srcfmt, void *src, ArrayDataFormat dstfmt, void *dst, size_t cnt) {
	uint8_t		*src8, *dst8;
	uint16_t	*src16, *dst16;

	dst8 = dst;
	dst16 = dst;
	src8 = src;
	src16 = src;

	/* We do this as a big switch in order to optimise efficiency for each transfer type */
	switch(dstfmt) {
	case ARRAY_DATA_4BITUNSIGNED:
		switch(srcfmt) {
		case ARRAY_DATA_4BITUNSIGNED:	if (dst != src) while(cnt--) { *dst8++ = *src8++; }		break;
		case ARRAY_DATA_4BITSIGNED:		while(cnt--) { *dst8++ = (*src8++ ^ 8); }				break;
		case ARRAY_DATA_8BITUNSIGNED:	while(cnt--) { *dst8++ = *src8++ >> 4; }				break;
		case ARRAY_DATA_8BITSIGNED:		while(cnt--) { *dst8++ = (*src8++ ^ 128) >> 4; }		break;
		case ARRAY_DATA_10BITUNSIGNED:	while(cnt--) { *dst8++ = *src16++ >> 6; }				break;
		case ARRAY_DATA_10BITSIGNED:	while(cnt--) { *dst8++ = (*src16++ ^ 512) >> 6; }		break;
		case ARRAY_DATA_12BITUNSIGNED:	while(cnt--) { *dst8++ = *src16++ >> 8; }				break;
		case ARRAY_DATA_12BITSIGNED:	while(cnt--) { *dst8++ = (*src16++ ^ 2048) >> 8; }		break;
		case ARRAY_DATA_14BITUNSIGNED:	while(cnt--) { *dst8++ = *src16++ >> 10; }				break;
		case ARRAY_DATA_14BITSIGNED:	while(cnt--) { *dst8++ = (*src16++ ^ 8192) >> 10; }		break;
		case ARRAY_DATA_16BITUNSIGNED:	while(cnt--) { *dst8++ = *src16++ >> 12; }				break;
		case ARRAY_DATA_16BITSIGNED:	while(cnt--) { *dst8++ = (*src16++ ^ 32768) >> 12; }	break;
		}
		break;
	case ARRAY_DATA_4BITSIGNED:
		switch(srcfmt) {
		case ARRAY_DATA_4BITUNSIGNED:	while(cnt--) { *dst8++ = (*src8++ ^ 8); }				break;
		case ARRAY_DATA_4BITSIGNED:		if (dst != src) while(cnt--) { *dst8++ = *src8++; }		break;
		case ARRAY_DATA_8BITUNSIGNED:	while(cnt--) { *dst8++ = (*src8++ ^ 128) >> 4; }		break;
		case ARRAY_DATA_8BITSIGNED:		while(cnt--) { *dst8++ = *src8++ >> 4; }				break;
		case ARRAY_DATA_10BITUNSIGNED:	while(cnt--) { *dst8++ = (*src16++ ^ 512) >> 6; }		break;
		case ARRAY_DATA_10BITSIGNED:	while(cnt--) { *dst8++ = *src16++ >> 6; }				break;
		case ARRAY_DATA_12BITUNSIGNED:	while(cnt--) { *dst8++ = (*src16++ ^ 2048) >> 8; }		break;
		case ARRAY_DATA_12BITSIGNED:	while(cnt--) { *dst8++ = *src16++ >> 8; }				break;
		case ARRAY_DATA_14BITUNSIGNED:	while(cnt--) { *dst8++ = (*src16++ ^ 8192) >> 10; }		break;
		case ARRAY_DATA_14BITSIGNED:	while(cnt--) { *dst8++ = *src16++ >> 10; }				break;
		case ARRAY_DATA_16BITUNSIGNED:	while(cnt--) { *dst8++ = (*src16++ ^ 32768) >> 12; }	break;
		case ARRAY_DATA_16BITSIGNED:	while(cnt--) { *dst8++ = *src16++ >> 12; }				break;
		}
		break;
	case ARRAY_DATA_8BITUNSIGNED:
		switch(srcfmt) {
		case ARRAY_DATA_4BITUNSIGNED:	while(cnt--) { *dst8++ = *src8++ << 4; }				break;
		case ARRAY_DATA_4BITSIGNED:		while(cnt--) { *dst8++ = (*src8++ ^ 8) << 4; }			break;
		case ARRAY_DATA_8BITUNSIGNED:	if (dst != src) while(cnt--) { *dst8++ = *src8++; }		break;
		case ARRAY_DATA_8BITSIGNED:		while(cnt--) { *dst8++ = (*src8++ ^ 128); }				break;
		case ARRAY_DATA_10BITUNSIGNED:	while(cnt--) { *dst8++ = *src16++ >> 2; }				break;
		case ARRAY_DATA_10BITSIGNED:	while(cnt--) { *dst8++ = (*src16++ ^ 512) >> 2; }		break;
		case ARRAY_DATA_12BITUNSIGNED:	while(cnt--) { *dst8++ = *src16++ >> 4; }				break;
		case ARRAY_DATA_12BITSIGNED:	while(cnt--) { *dst8++ = (*src16++ ^ 2048) >> 4; }		break;
		case ARRAY_DATA_14BITUNSIGNED:	while(cnt--) { *dst8++ = *src16++ >> 6; }				break;
		case ARRAY_DATA_14BITSIGNED:	while(cnt--) { *dst8++ = (*src16++ ^ 8192) >> 6; }		break;
		case ARRAY_DATA_16BITUNSIGNED:	while(cnt--) { *dst8++ = *src16++ >> 8; }				break;
		case ARRAY_DATA_16BITSIGNED:	while(cnt--) { *dst8++ = (*src16++ ^ 32768) >> 8; }		break;
		}
		break;
	case ARRAY_DATA_8BITSIGNED:
		switch(srcfmt) {
		case ARRAY_DATA_4BITUNSIGNED:	while(cnt--) { *dst8++ = (*src8++ ^ 8) << 4; }			break;
		case ARRAY_DATA_4BITSIGNED:		while(cnt--) { *dst8++ = *src8++ << 4; }				break;
		case ARRAY_DATA_8BITUNSIGNED:	while(cnt--) { *dst8++ = (*src8++ ^ 128); }				break;
		case ARRAY_DATA_8BITSIGNED:		if (dst != src) while(cnt--) { *dst8++ = *src8++; }		break;
		case ARRAY_DATA_10BITUNSIGNED:	while(cnt--) { *dst8++ = (*src16++ ^ 512) >> 2; }		break;
		case ARRAY_DATA_10BITSIGNED:	while(cnt--) { *dst8++ = *src16++ >> 2; }				break;
		case ARRAY_DATA_12BITUNSIGNED:	while(cnt--) { *dst8++ = (*src16++ ^ 2048) >> 4; }		break;
		case ARRAY_DATA_12BITSIGNED:	while(cnt--) { *dst8++ = *src16++ >> 4; }				break;
		case ARRAY_DATA_14BITUNSIGNED:	while(cnt--) { *dst8++ = (*src16++ ^ 8192) >> 6; }		break;
		case ARRAY_DATA_14BITSIGNED:	while(cnt--) { *dst8++ = *src16++ >> 6; }				break;
		case ARRAY_DATA_16BITUNSIGNED:	while(cnt--) { *dst8++ = (*src16++ ^ 32768) >> 8; }		break;
		case ARRAY_DATA_16BITSIGNED:	while(cnt--) { *dst8++ = *src16++ >> 8; }				break;
		}
		break;
	case ARRAY_DATA_10BITUNSIGNED:
		switch(srcfmt) {
		case ARRAY_DATA_4BITUNSIGNED:	while(cnt--) { *dst16++ = *src8++ << 6; }				break;
		case ARRAY_DATA_4BITSIGNED:		while(cnt--) { *dst16++ = (*src8++ ^ 8) << 6; }			break;
		case ARRAY_DATA_8BITUNSIGNED:	while(cnt--) { *dst16++ = *src8++ << 2; }				break;
		case ARRAY_DATA_8BITSIGNED:		while(cnt--) { *dst16++ = (*src8++ ^ 128) << 2; }		break;
		case ARRAY_DATA_10BITUNSIGNED:	if (dst != src) while(cnt--) { *dst16++ = *src16++; }	break;
		case ARRAY_DATA_10BITSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 512); }			break;
		case ARRAY_DATA_12BITUNSIGNED:	while(cnt--) { *dst16++ = *src16++ >> 2; }				break;
		case ARRAY_DATA_12BITSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 2048) >> 2; }		break;
		case ARRAY_DATA_14BITUNSIGNED:	while(cnt--) { *dst16++ = *src16++ >> 4; }				break;
		case ARRAY_DATA_14BITSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 8192) >> 4; }		break;
		case ARRAY_DATA_16BITUNSIGNED:	while(cnt--) { *dst16++ = *src16++ >> 6; }				break;
		case ARRAY_DATA_16BITSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 32768) >> 6; }	break;
		}
		break;
	case ARRAY_DATA_10BITSIGNED:
		switch(srcfmt) {
		case ARRAY_DATA_4BITUNSIGNED:	while(cnt--) { *dst16++ = (*src8++ ^ 8) << 6; }			break;
		case ARRAY_DATA_4BITSIGNED:		while(cnt--) { *dst16++ = *src8++ << 6; }				break;
		case ARRAY_DATA_8BITUNSIGNED:	while(cnt--) { *dst16++ = (*src8++ ^ 128) << 2; }		break;
		case ARRAY_DATA_8BITSIGNED:		while(cnt--) { *dst16++ = *src8++ << 2; }				break;
		case ARRAY_DATA_10BITUNSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 512); }			break;
		case ARRAY_DATA_10BITSIGNED:	if (dst != src) while(cnt--) { *dst16++ = *src16++; }	break;
		case ARRAY_DATA_12BITUNSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 2048) >> 2; }		break;
		case ARRAY_DATA_12BITSIGNED:	while(cnt--) { *dst16++ = *src16++ >> 2; }				break;
		case ARRAY_DATA_14BITUNSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 8192) >> 4; }		break;
		case ARRAY_DATA_14BITSIGNED:	while(cnt--) { *dst16++ = *src16++ >> 4; }				break;
		case ARRAY_DATA_16BITUNSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 32768) >> 6; }	break;
		case ARRAY_DATA_16BITSIGNED:	while(cnt--) { *dst16++ = *src16++ >> 6; }				break;
		}
		break;
	case ARRAY_DATA_12BITUNSIGNED:
		switch(srcfmt) {
		case ARRAY_DATA_4BITUNSIGNED:	while(cnt--) { *dst16++ = *src8++ << 8; }				break;
		case ARRAY_DATA_4BITSIGNED:		while(cnt--) { *dst16++ = (*src8++ ^ 8) << 8; }			break;
		case ARRAY_DATA_8BITUNSIGNED:	while(cnt--) { *dst16++ = *src8++ << 4; }				break;
		case ARRAY_DATA_8BITSIGNED:		while(cnt--) { *dst16++ = (*src8++ ^ 128) << 4; }		break;
		case ARRAY_DATA_10BITUNSIGNED:	while(cnt--) { *dst16++ = *src16++ << 2; }				break;
		case ARRAY_DATA_10BITSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 512) << 2; }		break;
		case ARRAY_DATA_12BITUNSIGNED:	if (dst != src) while(cnt--) { *dst16++ = *src16++; }	break;
		case ARRAY_DATA_12BITSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 2048); }			break;
		case ARRAY_DATA_14BITUNSIGNED:	while(cnt--) { *dst16++ = *src16++ >> 2; }				break;
		case ARRAY_DATA_14BITSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 8192) >> 2; }		break;
		case ARRAY_DATA_16BITUNSIGNED:	while(cnt--) { *dst16++ = *src16++ >> 4; }				break;
		case ARRAY_DATA_16BITSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 32768) >> 4; }	break;
		}
		break;
	case ARRAY_DATA_12BITSIGNED:
		switch(srcfmt) {
		case ARRAY_DATA_4BITUNSIGNED:	while(cnt--) { *dst16++ = (*src8++ ^ 8) << 8; }			break;
		case ARRAY_DATA_4BITSIGNED:		while(cnt--) { *dst16++ = *src8++ << 8; }				break;
		case ARRAY_DATA_8BITUNSIGNED:	while(cnt--) { *dst16++ = (*src8++ ^ 128) << 4; }		break;
		case ARRAY_DATA_8BITSIGNED:		while(cnt--) { *dst16++ = *src8++ << 4; }				break;
		case ARRAY_DATA_10BITUNSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 512) << 2; }		break;
		case ARRAY_DATA_10BITSIGNED:	while(cnt--) { *dst16++ = *src16++ << 2; }				break;
		case ARRAY_DATA_12BITUNSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 2048); }			break;
		case ARRAY_DATA_12BITSIGNED:	if (dst != src) while(cnt--) { *dst16++ = *src16++; }	break;
		case ARRAY_DATA_14BITUNSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 8192) >> 2; }		break;
		case ARRAY_DATA_14BITSIGNED:	while(cnt--) { *dst16++ = *src16++ >> 2; }				break;
		case ARRAY_DATA_16BITUNSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 32768) >> 4; }	break;
		case ARRAY_DATA_16BITSIGNED:	while(cnt--) { *dst16++ = *src16++ >> 4; }				break;
		}
		break;
	case ARRAY_DATA_14BITUNSIGNED:
		switch(srcfmt) {
		case ARRAY_DATA_4BITUNSIGNED:	while(cnt--) { *dst16++ = *src8++ << 10; }				break;
		case ARRAY_DATA_4BITSIGNED:		while(cnt--) { *dst16++ = (*src8++ ^ 8) << 10; }		break;
		case ARRAY_DATA_8BITUNSIGNED:	while(cnt--) { *dst16++ = *src8++ << 6; }				break;
		case ARRAY_DATA_8BITSIGNED:		while(cnt--) { *dst16++ = (*src8++ ^ 128) << 6; }		break;
		case ARRAY_DATA_10BITUNSIGNED:	while(cnt--) { *dst16++ = *src16++ << 4; }				break;
		case ARRAY_DATA_10BITSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 512) << 4; }		break;
		case ARRAY_DATA_12BITUNSIGNED:	while(cnt--) { *dst16++ = *src16++ << 2; }				break;
		case ARRAY_DATA_12BITSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 2048) >> 2; }		break;
		case ARRAY_DATA_14BITUNSIGNED:	if (dst != src) while(cnt--) { *dst16++ = *src16++; }	break;
		case ARRAY_DATA_14BITSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 8192); }			break;
		case ARRAY_DATA_16BITUNSIGNED:	while(cnt--) { *dst16++ = *src16++ >> 2; }				break;
		case ARRAY_DATA_16BITSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 32768) >> 2; }	break;
		}
		break;
	case ARRAY_DATA_14BITSIGNED:
		switch(srcfmt) {
		case ARRAY_DATA_4BITUNSIGNED:	while(cnt--) { *dst16++ = (*src8++ ^ 8) << 10; }		break;
		case ARRAY_DATA_4BITSIGNED:		while(cnt--) { *dst16++ = *src8++ << 10; }				break;
		case ARRAY_DATA_8BITUNSIGNED:	while(cnt--) { *dst16++ = (*src8++ ^ 128) << 6; }		break;
		case ARRAY_DATA_8BITSIGNED:		while(cnt--) { *dst16++ = *src8++ << 6; }				break;
		case ARRAY_DATA_10BITUNSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 512) << 4; }		break;
		case ARRAY_DATA_10BITSIGNED:	while(cnt--) { *dst16++ = *src16++ << 4; }				break;
		case ARRAY_DATA_12BITUNSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 2048) << 2; }		break;
		case ARRAY_DATA_12BITSIGNED:	while(cnt--) { *dst16++ = *src16++ << 2; }				break;
		case ARRAY_DATA_14BITUNSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 8192); }			break;
		case ARRAY_DATA_14BITSIGNED:	if (dst != src) while(cnt--) { *dst16++ = *src16++; }	break;
		case ARRAY_DATA_16BITUNSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 32768) >> 2; }	break;
		case ARRAY_DATA_16BITSIGNED:	while(cnt--) { *dst16++ = *src16++ >> 2; }				break;
		}
		break;
	case ARRAY_DATA_16BITUNSIGNED:
		switch(srcfmt) {
		case ARRAY_DATA_4BITUNSIGNED:	while(cnt--) { *dst16++ = *src8++ << 12; }				break;
		case ARRAY_DATA_4BITSIGNED:		while(cnt--) { *dst16++ = (*src8++ ^ 8) << 12; }		break;
		case ARRAY_DATA_8BITUNSIGNED:	while(cnt--) { *dst16++ = *src8++ << 8; }				break;
		case ARRAY_DATA_8BITSIGNED:		while(cnt--) { *dst16++ = (*src8++ ^ 128) << 8; }		break;
		case ARRAY_DATA_10BITUNSIGNED:	while(cnt--) { *dst16++ = *src16++ << 6; }				break;
		case ARRAY_DATA_10BITSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 512) << 6; }		break;
		case ARRAY_DATA_12BITUNSIGNED:	while(cnt--) { *dst16++ = *src16++ << 4; }				break;
		case ARRAY_DATA_12BITSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 2048) >> 4; }		break;
		case ARRAY_DATA_14BITUNSIGNED:	while(cnt--) { *dst16++ = *src16++ >> 2; }				break;
		case ARRAY_DATA_14BITSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 8192) >> 2; }		break;
		case ARRAY_DATA_16BITUNSIGNED:	if (dst != src) while(cnt--) { *dst16++ = *src16++; }	break;
		case ARRAY_DATA_16BITSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 32768); }			break;
		}
		break;
	case ARRAY_DATA_16BITSIGNED:
		switch(srcfmt) {
		case ARRAY_DATA_4BITUNSIGNED:	while(cnt--) { *dst16++ = (*src8++ ^ 8) << 12; }		break;
		case ARRAY_DATA_4BITSIGNED:		while(cnt--) { *dst16++ = *src8++ << 12; }				break;
		case ARRAY_DATA_8BITUNSIGNED:	while(cnt--) { *dst16++ = (*src8++ ^ 128) << 8; }		break;
		case ARRAY_DATA_8BITSIGNED:		while(cnt--) { *dst16++ = *src8++ << 8; }				break;
		case ARRAY_DATA_10BITUNSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 512) << 6; }		break;
		case ARRAY_DATA_10BITSIGNED:	while(cnt--) { *dst16++ = *src16++ << 6; }				break;
		case ARRAY_DATA_12BITUNSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 2048) << 4; }		break;
		case ARRAY_DATA_12BITSIGNED:	while(cnt--) { *dst16++ = *src16++ << 4; }				break;
		case ARRAY_DATA_14BITUNSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 8192) << 2; }		break;
		case ARRAY_DATA_14BITSIGNED:	while(cnt--) { *dst16++ = *src16++ >> 2; }				break;
		case ARRAY_DATA_16BITUNSIGNED:	while(cnt--) { *dst16++ = (*src16++ ^ 32768); }			break;
		case ARRAY_DATA_16BITSIGNED:	if (dst != src) while(cnt--) { *dst16++ = *src16++; }	break;
		}
		break;
	}
}

#endif /* GFX_USE_GMISC && GMISC_NEED_ARRAYOPS */
