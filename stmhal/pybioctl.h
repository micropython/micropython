#define MP_IOCTL_POLL (0x100 | 1)

// These values are compatible with Linux, which are in turn
// compatible with iBCS2 spec.
#define MP_IOCTL_POLL_RD  (0x0001)
#define MP_IOCTL_POLL_WR  (0x0004)
#define MP_IOCTL_POLL_ERR (0x0008)
#define MP_IOCTL_POLL_HUP (0x0010)
