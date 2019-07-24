/*
 * Amazon FreeRTOS+POSIX V1.0.2
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

/**
 * @file errno.h
 * @brief System error numbers.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/errno.h.html
 *
 * The values defined in this file may not be compatible with the strerror
 * function provided by this system.
 */

#ifndef _FREERTOS_POSIX_ERRNO_H_
#define _FREERTOS_POSIX_ERRNO_H_

/* Undefine all errnos to avoid redefinition errors with system errnos. */
#undef EPERM
#undef ENOENT
#undef EBADF
#undef EAGAIN
#undef ENOMEM
#undef EEXIST
#undef EBUSY
#undef EINVAL
#undef ENOSPC
#undef ERANGE
#undef ENAMETOOLONG
#undef EDEADLK
#undef EOVERFLOW
#undef ENOSYS
#undef EMSGSIZE
#undef ENOTSUP
#undef ETIMEDOUT

/**
 * @defgroup Definition of POSIX errnos.
 */
/**@{ */
#define EPERM        1   /**< Operation not permitted. */
#define ENOENT       2   /**< No such file or directory. */
#define EBADF        9   /**< Bad file descriptor. */
#define EAGAIN       11  /**< Resource unavailable, try again. */
#define ENOMEM       12  /**< Not enough space. */
#define EEXIST       17  /**< File exists. */
#define EBUSY        16  /**< Device or resource busy. */
#define EINVAL       22  /**< Invalid argument. */
#define ENOSPC       28  /**< No space left on device. */
#define ERANGE       34  /**< Result too large. */
#define ENAMETOOLONG 36  /**< File name too long. */
#define EDEADLK      45  /**< Resource deadlock would occur. */
#define EOVERFLOW    75  /**< Value too large to be stored in data type. */
#define ENOSYS       88  /**< Function not supported. */
#define EMSGSIZE     90  /**< Message too long. */
#define ENOTSUP      95  /**< Operation not supported. */
#define ETIMEDOUT    116 /**< Connection timed out. */
/**@} */

/**
 * @brief Define FreeRTOS+POSIX errno, if enabled.
 */
#if ( configUSE_POSIX_ERRNO == 1 )

/**
 * @brief System error variable, errno.
 */
    extern int FreeRTOS_errno;
    #define errno FreeRTOS_errno
#endif

#endif /* ifndef _FREERTOS_POSIX_ERRNO_H_ */
