/*
* This file is part of the Micro Python project, http://micropython.org/
*
* The MIT License (MIT)
*
* Copyright (c) 2013, 2014 Damien P. George
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

#include "mpconfig.h"
#include "misc.h"
#include "nlr.h"
#include "qstr.h"
#include "obj.h"
#include <windows.h>

IMAGE_NT_HEADERS *header_from_memory(const char *module) {
    BYTE *base_addr = (BYTE*)GetModuleHandleA(module);
    IMAGE_DOS_HEADER *dos_header = (IMAGE_DOS_HEADER*)base_addr;
    return (IMAGE_NT_HEADERS*)(base_addr + dos_header->e_lfanew);
}

IMAGE_SECTION_HEADER *find_section(IMAGE_NT_HEADERS *nt_header, const char *name) {
    int i;
    IMAGE_SECTION_HEADER *section = IMAGE_FIRST_SECTION(nt_header);
    for (i = 0; i < nt_header->FileHeader.NumberOfSections; ++i) {
        if (strcmp((const char *)section->Name, name) == 0) {
            return section;
        }
        ++section;
    }
    return NULL;
}

void section_boundaries(IMAGE_NT_HEADERS *nt_header, IMAGE_SECTION_HEADER *section, char **start, char **end) {
    if (section == NULL) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Could not lookup section boundaries"));
    }
    *start = (char*)(nt_header->OptionalHeader.ImageBase + section->VirtualAddress);
    *end = *start + section->Misc.VirtualSize;    
}

void section_boundaries_from_module(const char *module, const char *section, char **start, char **end) {
    IMAGE_NT_HEADERS *nt_header = header_from_memory(module);
    IMAGE_SECTION_HEADER *dsection = find_section(nt_header, section);
    section_boundaries(nt_header, dsection, start, end);   
}

char *bss_start = 0;
char *bss_end = 0;

//MSVC has no __bss_start and _end but we can get accurate section info from the PE header.
//The standard .bss section is appended to the standard .data section however so it cannot
//be looked up by name. To deal with that we put all uPy static variables in a named section.
void getbss() {
    section_boundaries_from_module(NULL, MICROPY_PORT_BSSSECTION, &bss_start, &bss_end);
}
