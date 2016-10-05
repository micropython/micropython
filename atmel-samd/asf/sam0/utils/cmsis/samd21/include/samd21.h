/**
 * \file
 *
 * \brief Top header file for SAMD21
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _SAMD21_
#define _SAMD21_

/**
 * \defgroup SAMD21_definitions SAMD21 Device Definitions
 * \brief SAMD21 CMSIS Definitions.
 */

#if   defined(__SAMD21E15A__) || defined(__ATSAMD21E15A__)
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
#elif defined (__SAMD21G18AU__) || defined(__ATSAMD21G18AU__)
  #include "samd21g18au.h"
#elif defined(__SAMD21J15A__) || defined(__ATSAMD21J15A__)
  #include "samd21j15a.h"
#elif defined(__SAMD21J16A__) || defined(__ATSAMD21J16A__)
  #include "samd21j16a.h"
#elif defined(__SAMD21J17A__) || defined(__ATSAMD21J17A__)
  #include "samd21j17a.h"
#elif defined(__SAMD21J18A__) || defined(__ATSAMD21J18A__)
  #include "samd21j18a.h"
#elif defined(__SAMD21E15B__) || defined(__ATSAMD21E15B__)
  #include "samd21e15b.h"
#elif defined(__SAMD21E15BU__) || defined(__ATSAMD21E15BU__)
  #include "samd21e15bu.h"
#elif defined(__SAMD21E15L__) || defined(__ATSAMD21E15L__)
  #include "samd21e15l.h"
#elif defined(__SAMD21E16B__) || defined(__ATSAMD21E16B__)
  #include "samd21e16b.h"
#elif defined(__SAMD21E16BU__) || defined(__ATSAMD21E16BU__)
  #include "samd21e16bu.h"
#elif defined(__SAMD21E16L__) || defined(__ATSAMD21E16L__)
  #include "samd21e16l.h"
#elif defined(__SAMD21G15B__) || defined(__ATSAMD21G15B__)
  #include "samd21g15b.h"
#elif defined(__SAMD21G15L__) || defined(__ATSAMD21G15L__)
  #include "samd21g15l.h"
#elif defined(__SAMD21G16B__) || defined(__ATSAMD21G16B__)
  #include "samd21g16b.h"
#elif defined(__SAMD21G16L__) || defined(__ATSAMD21G16L__)
  #include "samd21g16l.h"
#elif defined(__SAMD21J15B__) || defined(__ATSAMD21J15B__)
  #include "samd21j15b.h"
#elif defined(__SAMD21J16B__) || defined(__ATSAMD21J16B__)
  #include "samd21j16b.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAMD21_ */
