/**
  Copyright (c) 2013-2015 Particle Industries, Inc.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */

#ifndef SPARK_MACROS_H_
#define SPARK_MACROS_H_

#if !defined(arraySize)
#   define arraySize(a)            (sizeof((a))/sizeof((a[0])))
#endif

#define INVALID_CASE(c) PANIC(InvalidCase,"Invalid Case %d",(c))
#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif

#define _CAT(a, b) a ## b
#define CAT(a, b) _CAT(a, b)

#define CCASSERT(predicate) _x_CCASSERT_LINE(predicate, __LINE__)
#define _x_CCASSERT_LINE(predicate, line) typedef char CAT(constraint_violated_on_line_,line)[2*((predicate)!=0)-1];

#ifndef stringify
#define stringify(x) __stringify(x)
#define __stringify(x) #x
#endif

// Seconds to Us
#define S2u(s) ((s)*1000000)
// Mili Seconds to Us
#define MS2u(m) ((m)*1000)

// Seconds to Ms
#define S2M(s) ((s)*1000)

#define BYTE_N(x,n)			        ((x) >> ((n)*8) & 0x000000FF)

#endif /* SPARK_MACROS_H_ */
