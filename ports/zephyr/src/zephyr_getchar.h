/*
 * Copyright (c) 2016 Linaro
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

#ifndef MICROPY_INCLUDED_ZEPHYR_SRC_ZEPHYR_GETCHAR_H
#define MICROPY_INCLUDED_ZEPHYR_SRC_ZEPHYR_GETCHAR_H

#ifdef CONFIG_MICROPY_GETCHAR_CONSOLE_DRIVER

void zephyr_getchar_init(void);
int zephyr_getchar_check(void);
int zephyr_getchar(void);

#endif

#endif /* MICROPY_INCLUDED_ZEPHYR_SRC_ZEPHYR_GETCHAR_H */
