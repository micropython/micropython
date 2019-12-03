#ifndef ASSERT_H
#define ASSERT_H

#ifndef NDEBUG
void __assert_func(const char *file, int line, const char *func, const char *expr);
#define assert(expr) ((expr) ? (void)0 : __assert_func(__FILE__, __LINE__, __func__, #expr))
#else
#define assert(x)
#endif

#endif
