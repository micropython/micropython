#include <linux/kernel.h> // snprintf

#define printf(...) printk(__VA_ARGS__)
