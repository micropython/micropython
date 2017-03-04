/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef SDK_CONFIG_H__
#define SDK_CONFIG_H__

#define MEMORY_MANAGER_SMALL_BLOCK_COUNT                 (8)
#define MEMORY_MANAGER_SMALL_BLOCK_SIZE                  (128)
#define MEMORY_MANAGER_MEDIUM_BLOCK_COUNT                (4)
#define MEMORY_MANAGER_MEDIUM_BLOCK_SIZE                 (256)
#define MEMORY_MANAGER_LARGE_BLOCK_COUNT                 (3)
#define MEMORY_MANAGER_LARGE_BLOCK_SIZE                  (1024)
#define MEM_MANAGER_ENABLE_LOGS                          (0)
#define MEM_MANAGER_DISABLE_API_PARAM_CHECK              (0)
#define IOT_CONTEXT_MANAGER_ENABLE_LOGS                  (0)
#define IOT_CONTEXT_MANAGER_DISABLE_API_PARAM_CHECK      (0)
#define IOT_CONTEXT_MANAGER_MAX_CONTEXTS                 (16)
#define IOT_CONTEXT_MANAGER_MAX_TABLES                   (1)
#define NRF_LWIP_DRIVER_ENABLE_LOGS                      (0)
#define IOT_TIMER_RESOLUTION_IN_MS                       (100)
#define IOT_TIMER_DISABLE_API_PARAM_CHECK                (0)

#endif // SDK_CONFIG_H__
