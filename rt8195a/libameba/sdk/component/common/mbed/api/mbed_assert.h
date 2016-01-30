/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MBED_ASSERT_H
#define MBED_ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

/** Internal mbed assert function which is invoked when MBED_ASSERT macro failes.
 *  This function is active only if NDEBUG is not defined prior to including this
 *  assert header file.
 *  In case of MBED_ASSERT failing condition, the assertation message is printed
 *  to stderr and mbed_die() is called.
 *  @param expr Expresion to be checked.
 *  @param file File where assertation failed.
 *  @param line Failing assertation line number.
 */
void mbed_assert_internal(const char *expr, const char *file, int line);

#ifdef __cplusplus
}
#endif

#ifdef NDEBUG
#define MBED_ASSERT(expr) ((void)0)

#else
#define MBED_ASSERT(expr)                                \
do {                                                     \
    if (!(expr)) {                                       \
        mbed_assert_internal(#expr, __FILE__, __LINE__); \
    }                                                    \
} while (0)
#endif

#endif
