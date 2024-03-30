// Define so there's no dependency on extmod/virtpin.h
#define mp_hal_pin_obj_t mp_obj_t

#if MICROPY_KBD_EXCEPTION
void mp_hal_set_interrupt_char(int c);
#endif

#if MICROPY_VFS_POSIX
// This macro is used to implement PEP 475 to retry specified syscalls on EINTR
#define MP_HAL_RETRY_SYSCALL(ret, syscall, raise) { \
        for (;;) { \
            MP_THREAD_GIL_EXIT(); \
            ret = syscall; \
            MP_THREAD_GIL_ENTER(); \
            if (ret == -1) { \
                int err = errno; \
                if (err == EINTR) { \
                    mp_handle_pending(true); \
                    continue; \
                } \
                raise; \
            } \
            break; \
        } \
}
#endif
