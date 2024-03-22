The following are daily builds of the ESP8266 firmware. This will work on
boards with at least 2MiB of flash.  They have the latest features and bug
fixes, WebREPL is not automatically started, and debugging is enabled by
default.

For boards with 1MiB or 512kiB of flash, two variants are provided with reduced
functionality. The 1MiB variant removes asyncio and FAT-filesystem support as
well as some modules from micropython-lib. The 512kiB variant further removes
all filesystem support, as well as framebuffer support, some Python language
features, and has less detailed error messages.

Note: v1.12-334 and newer (including v1.13) require an ESP8266 module with
2MiB of flash or more, and use littlefs as the filesystem by default.  When
upgrading from older firmware please backup your files first, and either
erase all flash before upgrading, or after upgrading execute
`vfs.VfsLfs2.mkfs(bdev)`.

### OTA builds
Over-The-Air (OTA) builds of the ESP8266 firmware are also provided.

The first time you use this build you need to flash one of the "initial image"
images using esptool.py as described above.  After that, you can update the
firmware over the air using the "OTA update" file in conjunction with the
ota-client script from yaota8266. The "OTA update" files are digitally signed
and will only work with the provided "initial image" files, and vice versa.
(Note: this feature is work-in-progress.)
