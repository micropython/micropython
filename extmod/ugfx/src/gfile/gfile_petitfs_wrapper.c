/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "../../gfx.h"

#if GFX_USE_GFILE && GFILE_NEED_PETITFS && !GFILE_PETITFS_EXTERNAL_LIB

#include "gfile_petitfs_wrapper.h"

// Include the source we want
#include "../../3rdparty/petitfs-0.03/src/pff.c"

#endif // GFX_USE_GFILE && GFILE_NEED_PETITFS && !GFILE_PETITFS_EXTERNAL_LIB
