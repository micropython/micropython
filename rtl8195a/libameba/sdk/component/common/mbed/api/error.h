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
#ifndef MBED_ERROR_H
#define MBED_ERROR_H

/** To generate a fatal compile-time error, you can use the pre-processor #error directive.
 *
 * @code
 * #error "That shouldn't have happened!"
 * @endcode
 *
 * If the compiler evaluates this line, it will report the error and stop the compile.
 *
 * For example, you could use this to check some user-defined compile-time variables:
 *
 * @code
 * #define NUM_PORTS 7
 * #if (NUM_PORTS > 4)
 *     #error "NUM_PORTS must be less than 4"
 * #endif
 * @endcode
 *
 * Reporting Run-Time Errors:
 * To generate a fatal run-time error, you can use the mbed error() function.
 *
 * @code
 * error("That shouldn't have happened!");
 * @endcode
 *
 * If the mbed running the program executes this function, it will print the
 * message via the USB serial port, and then die with the blue lights of death!
 *
 * The message can use printf-style formatting, so you can report variables in the
 * message too. For example, you could use this to check a run-time condition:
 *
 * @code
 * if(x >= 5) {
 *     error("expected x to be less than 5, but got %d", x);
 * }
 * #endcode
 */

#ifdef __cplusplus
extern "C" {
#endif

void error(const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif
