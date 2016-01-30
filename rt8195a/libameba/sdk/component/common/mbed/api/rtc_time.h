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

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Implementation of the C time.h functions
 *
 * Provides mechanisms to set and read the current time, based
 * on the microcontroller Real-Time Clock (RTC), plus some
 * standard C manipulation and formating functions.
 *
 * Example:
 * @code
 * #include "mbed.h"
 *
 * int main() {
 *     set_time(1256729737);  // Set RTC time to Wed, 28 Oct 2009 11:35:37
 *
 *     while(1) {
 *         time_t seconds = time(NULL);
 *
 *         printf("Time as seconds since January 1, 1970 = %d\n", seconds);
 *
 *         printf("Time as a basic string = %s", ctime(&seconds));
 *
 *         char buffer[32];
 *         strftime(buffer, 32, "%I:%M %p\n", localtime(&seconds));
 *         printf("Time as a custom formatted string = %s", buffer);
 *
 *         wait(1);
 *     }
 * }
 * @endcode
 */

/** Set the current time
 *
 * Initialises and sets the time of the microcontroller Real-Time Clock (RTC)
 * to the time represented by the number of seconds since January 1, 1970
 * (the UNIX timestamp).
 *
 * @param t Number of seconds since January 1, 1970 (the UNIX timestamp)
 *
 * Example:
 * @code
 * #include "mbed.h"
 *
 * int main() {
 *     set_time(1256729737); // Set time to Wed, 28 Oct 2009 11:35:37
 * }
 * @endcode
 */
void set_time(time_t t);

#ifdef __cplusplus
}
#endif
