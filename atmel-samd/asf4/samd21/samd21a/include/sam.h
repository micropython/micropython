/**
 * \file
 *
 * \brief Top level header file
 *
 * Copyright (c) 2017 Atmel Corporation, a wholly owned subsidiary of Microchip Technology Inc.
 *
 * \license_start
 *
 * \page License
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \license_stop
 *
 */

#ifndef _SAM_
#define _SAM_

#if defined(__SAMD21E15A__) || defined(__ATSAMD21E15A__)
#include "samd21e15a.h"
#elif defined(__SAMD21E16A__) || defined(__ATSAMD21E16A__)
#include "samd21e16a.h"
#elif defined(__SAMD21E17A__) || defined(__ATSAMD21E17A__)
#include "samd21e17a.h"
#elif defined(__SAMD21E18A__) || defined(__ATSAMD21E18A__)
#include "samd21e18a.h"
#elif defined(__SAMD21G15A__) || defined(__ATSAMD21G15A__)
#include "samd21g15a.h"
#elif defined(__SAMD21G16A__) || defined(__ATSAMD21G16A__)
#include "samd21g16a.h"
#elif defined(__SAMD21G17A__) || defined(__ATSAMD21G17A__)
#include "samd21g17a.h"
#elif defined(__SAMD21G17AU__) || defined(__ATSAMD21G17AU__)
#include "samd21g17au.h"
#elif defined(__SAMD21G18A__) || defined(__ATSAMD21G18A__)
#include "samd21g18a.h"
#elif defined(__SAMD21G18AU__) || defined(__ATSAMD21G18AU__)
#include "samd21g18au.h"
#elif defined(__SAMD21J15A__) || defined(__ATSAMD21J15A__)
#include "samd21j15a.h"
#elif defined(__SAMD21J16A__) || defined(__ATSAMD21J16A__)
#include "samd21j16a.h"
#elif defined(__SAMD21J17A__) || defined(__ATSAMD21J17A__)
#include "samd21j17a.h"
#elif defined(__SAMD21J18A__) || defined(__ATSAMD21J18A__)
#include "samd21j18a.h"
#else
#error Library does not support the specified device
#endif

#endif /* _SAM_ */
