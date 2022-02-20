For deploying the firmware use the Teensy Loader tool provided by PJRC on their web site.
PJRC provides both a graphical and command line version for Windows, OS X and Linux.
With the command line version, run the command:

```bash
teensy_loader_cli --mcu=imxrt1062 -v -w TEENSY40-<date_version_tag>.hex
```
or for Teensy 4.1:

```bash
teensy_loader_cli --mcu=imxrt1062 -v -w TEENSY41-<date_version_tag>.hex
```

When loading the firmware the PJRC boot loader will erase the content of the board file system.
