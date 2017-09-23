/**
 * \file
 *
 * \brief Top header file for SAMD51
 *
 * Copyright (c) 2017 Atmel Corporation,
 *                    a wholly owned subsidiary of Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMD51_
#define _SAMD51_

/**
 * \defgroup SAMD51_definitions SAMD51 Device Definitions
 * \brief SAMD51 CMSIS Definitions.
 */

#if   defined(__SAMD51G18A__) || defined(__ATSAMD51G18A__)
  #include "samd51g18a.h"
#elif defined(__SAMD51G19A__) || defined(__ATSAMD51G19A__)
  #include "samd51g19a.h"
#elif defined(__SAMD51J18A__) || defined(__ATSAMD51J18A__)
  #include "samd51j18a.h"
#elif defined(__SAMD51J19A__) || defined(__ATSAMD51J19A__)
  #include "samd51j19a.h"
#elif defined(__SAMD51J20A__) || defined(__ATSAMD51J20A__)
  #include "samd51j20a.h"
#elif defined(__SAMD51N19A__) || defined(__ATSAMD51N19A__)
  #include "samd51n19a.h"
#elif defined(__SAMD51N20A__) || defined(__ATSAMD51N20A__)
  #include "samd51n20a.h"
#elif defined(__SAMD51P19A__) || defined(__ATSAMD51P19A__)
  #include "samd51p19a.h"
#elif defined(__SAMD51P20A__) || defined(__ATSAMD51P20A__)
  #include "samd51p20a.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAMD51_ */
