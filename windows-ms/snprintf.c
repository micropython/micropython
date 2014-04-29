#include <stdarg.h>
#include <stdio.h>
#include <malloc.h>

// _snprintf/vsnprintf are fine otherwise, except the 'F' specifier is not handled
int snprintf(char *dest, size_t count, const char *format, ...) {
    const size_t fmtLen = strlen(format) + 1;
    char *fixedFmt = alloca(fmtLen);
    for (size_t i = 0; i < fmtLen; ++i)
        fixedFmt[i] = format[i] == 'F' ? 'f' : format[i];

    va_list args;
    va_start(args, format);
    const int ret = vsnprintf(dest, count, fixedFmt, args);
    va_end(args);

    return ret;
}
