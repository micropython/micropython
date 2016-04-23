/*
 * File:   static_assert.h
 * Author: mat
 *
 * Created on 16 December 2014, 04:06
 */

#ifndef STATIC_ASSERT_H
#define	STATIC_ASSERT_H

#ifdef	__cplusplus
extern "C" {
#endif

#define STATIC_ASSERT_EXPR(name, condition) ((void)sizeof(char[1 - 2*!!(condition)]))

#if defined(__cplusplus) && __cplusplus >= 201103L
#define STATIC_ASSERT(name,condition) static_assert(condition,#name)
#else
#define STATIC_ASSERT(name,condition) typedef char assert_##name[(condition)?0:-1]
#endif


#ifdef	__cplusplus
}
#endif

#endif	/* STATIC_ASSERT_H */

