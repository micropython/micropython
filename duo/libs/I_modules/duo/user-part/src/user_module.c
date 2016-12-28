/**
 * The user module export table
 */

#include "dynalib.h"

DYNALIB_TABLE_EXTERN(user);

/**
 * The module export table. This lists the addresses of individual library dynalib jump tables.
 */
__attribute__((externally_visible)) const void* const user_part_module[] = {
    DYNALIB_TABLE_NAME(user),
};

