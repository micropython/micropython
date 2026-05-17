// This file provides a version of __errno() for embedded systems that do not have one.
// This function is needed for expressions of the form: &errno

static int embed_errno;

#if defined(__linux__)
int *__errno_location(void)
#else
int *__errno(void)
#endif
{
    return &embed_errno;
}
