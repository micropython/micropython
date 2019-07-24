/*
 * Amazon FreeRTOS
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */
#ifndef AWS_DEFENDER_UTILS_H
#define AWS_DEFENDER_UTILS_H

#include "aws_defender_report_types.h"

/**
 * @brief Selects which tag to use for current build
 *
 * It's recommended to use the -D flag to set the definition to ensure
 * consistent compilation results. e.g. gcc -D DEFENDER_USE_LONG_TAG
 *
 * @param  long_tag The long form of the tag e.g. header
 * @param  short_tag The short form of the tag e.g. hed
 * @return          IF DEFENDER_USE_LONG_TAG is defined, then the long tag is
 *                  returned.  Otherwise the short tag is returned.
 *
 */
#ifdef DEFENDER_USE_SHORT_TAG
    #define DEFENDER_SelectTag( long_tag, short_tag )    ( short_tag )
#elif defined DEFENDER_USE_LONG_TAG
    #define DEFENDER_SelectTag( long_tag, short_tag )    ( long_tag )
#elif defined NDEBUG
    #define DEFENDER_SelectTag( long_tag, short_tag )    ( short_tag )
#else
    #define DEFENDER_SelectTag( long_tag, short_tag )    ( long_tag )
#endif

/**
 * @brief Returns the current state of the device defender agent
 * @note used for testing, and not meant to be used as part of public API
 * @return  DefenderState_t The current agent state
 */
DefenderState_t StateGet( void );

/**
 * @brief Converts state enum to a string
 * @param  eState State enum value
 * @return        Stringified enum
 */
char * StateAsString( DefenderState_t /*eState*/ );


#endif /* end of include guard: AWS_DEFENDER_UTILS_H */
