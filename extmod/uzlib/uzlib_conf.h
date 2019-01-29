/*
 * uzlib  -  tiny deflate/inflate library (deflate, gzip, zlib)
 *
 * Copyright (c) 2014-2018 by Paul Sokolovsky
 */

#ifndef UZLIB_CONF_H_INCLUDED
#define UZLIB_CONF_H_INCLUDED

#ifndef UZLIB_CONF_DEBUG_LOG
/* Debug logging level 0, 1, 2, etc. */
#define UZLIB_CONF_DEBUG_LOG 0
#endif

#ifndef UZLIB_CONF_PARANOID_CHECKS
/* Perform extra checks on the input stream, even if they aren't proven
   to be strictly required (== lack of them wasn't proven to lead to
   crashes). */
#define UZLIB_CONF_PARANOID_CHECKS 0
#endif

#endif /* UZLIB_CONF_H_INCLUDED */
