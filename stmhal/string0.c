#include <stdint.h>
#include "std.h"

void *memcpy(void *dest, const void *src, size_t n) {
    // TODO align and copy 32 bits at a time
    uint8_t *d = dest;
    const uint8_t *s = src;
    for (; n > 0; n--) {
        *d++ = *s++;
    }
    return dest;
}

void *memmove(void *dest, const void *src, size_t n) {
    if (src < dest && dest < src + n) {
        // need to copy backwards
        uint8_t *d = dest + n - 1;
        const uint8_t *s = src + n - 1;
        for (; n > 0; n--) {
            *d-- = *s--;
        }
        return dest;
    } else {
        // can use normal memcpy
        return memcpy(dest, src, n);
    }
}

void *memset(void *s, int c, size_t n) {
    uint8_t *s2 = s;
    for (; n > 0; n--) {
        *s2++ = c;
    }
    return s;
}

int memcmp(const char *s1, const char *s2, size_t n) {
    while (n--) {
        char c1 = *s1++;
        char c2 = *s2++;
        if (c1 < c2) return -1;
        else if (c1 > c2) return 1;
    }
    return 0;
}

size_t strlen(const char *str) {
    int len = 0;
    for (const char *s = str; *s; s++) {
        len += 1;
    }
    return len;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        char c1 = *s1++; // XXX UTF8 get char, next char
        char c2 = *s2++; // XXX UTF8 get char, next char
        if (c1 < c2) return -1;
        else if (c1 > c2) return 1;
    }
    if (*s2) return -1;
    else if (*s1) return 1;
    else return 0;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while (*s1 && *s2 && n > 0) {
        char c1 = *s1++; // XXX UTF8 get char, next char
        char c2 = *s2++; // XXX UTF8 get char, next char
        n--;
        if (c1 < c2) return -1;
        else if (c1 > c2) return 1;
    }
    if (n == 0) return 0;
    else if (*s2) return -1;
    else if (*s1) return 1;
    else return 0;
}

char *strcpy(char *dest, const char *src) {
    char *d = dest;
    while (*src) {
        *d++ = *src++;
    }
    *d = '\0';
    return dest;
}

// needed because gcc optimises strcpy + strcat to this
char *stpcpy(char *dest, const char *src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
    return dest;
}

char *strcat(char *dest, const char *src) {
    char *d = dest;
    while (*d) {
        d++;
    }
    while (*src) {
        *d++ = *src++;
    }
    *d = '\0';
    return dest;
}

// Public Domain implementation of strchr from:
// http://en.wikibooks.org/wiki/C_Programming/Strings#The_strchr_function
char *strchr(const char *s, int c)
{
    /* Scan s for the character.  When this loop is finished,
       s will either point to the end of the string or the
       character we were looking for.  */
    while (*s != '\0' && *s != (char)c)
        s++;
    return ((*s == c) ? (char *) s : 0);
}


// Public Domain implementation of strstr from:
// http://en.wikibooks.org/wiki/C_Programming/Strings#The_strstr_function
char *strstr(const char *haystack, const char *needle)
{
    size_t needlelen;
    /* Check for the null needle case.  */
    if (*needle == '\0')
        return (char *) haystack;
    needlelen = strlen(needle);
    for (; (haystack = strchr(haystack, *needle)) != 0; haystack++)
        if (strncmp(haystack, needle, needlelen) == 0)
            return (char *) haystack;
    return 0;
}
