
#define DYNALIB_EXPORT
#include "module_user_init.h"
#include "system_user.h"
#include <stddef.h>
#include <string.h>
#include "core_hal.h"


extern char link_heap_start;

/**
 * Locations of static memory regions from linker.
 */
extern char link_global_data_initial_values;
extern char link_global_data_start;
extern char link_global_data_end;
#define link_global_data_size (&link_global_data_end - &link_global_data_start)

extern char link_bss_location;
extern char link_bss_end;
#define link_bss_size (&link_bss_end - &link_bss_location)

extern char link_end_of_static_ram;


/**
 * Array of C++ static constructors.
 */
typedef void  (*constructor_ptr_t)(void);
extern constructor_ptr_t link_constructors_location[];
extern constructor_ptr_t link_constructors_end;

/**
 * Initializes this user module. Returns the start of the heap.
 */
void* module_user_pre_init() {

    if ( (&link_global_data_start!=&link_global_data_initial_values) && (link_global_data_size != 0))
    {
        memcpy(&link_global_data_start, &link_global_data_initial_values, link_global_data_size);
    }

    memset(&link_bss_location, 0, link_bss_size );
    return &link_heap_start;
}

/**
 * Array of C++ static constructors.
 */
typedef void  (*constructor_ptr_t)(void);
extern constructor_ptr_t link_constructors_location[];
extern constructor_ptr_t link_constructors_end;
#define link_constructors_size   ((unsigned long)&link_constructors_end  -  (unsigned long)&link_constructors_location )

void module_user_init()
{
    module_user_init_hook();

    // invoke constructors
    int ctor_num;
    for (ctor_num=0; ctor_num < link_constructors_size/sizeof(constructor_ptr_t); ctor_num++ )
    {
        link_constructors_location[ctor_num]();
    }
}

/**
 * Export these functions with a fuller name so they don't clash with the setup/loop wrappers in the system module.
 */
void module_user_setup() {
    setup();
}

void module_user_loop() {
    loop();
    serialEventRun();
}

#include "user_dynalib.h"

