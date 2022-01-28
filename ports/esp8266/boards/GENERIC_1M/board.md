The following are daily builds of the ESP8266 firmware tailored for modules with
only 1MiB of flash.  This firmware uses littlefs as the filesystem.
When upgrading from older firmware that uses a FAT filesystem please backup your files
first, and either erase all flash before upgrading, or after upgrading execute
`uos.VfsLfs2.mkfs(bdev)`.
