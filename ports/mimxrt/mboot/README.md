# Memory Layout

## QSPI
```
            +--------------+
0x6000'0000 |  mboot       |
            |              |
            +--------------+
0x6000'8000 |  firmware    |
            |              |
            |              |
            +--------------+
0x6010'0000 |  VFS         |
            |              |
            /  ...         /
            |              |
            +--------------+
```

## HyperFlash
```
            +--------------+
0x6000'0000 |  mboot       |
            |              |
            +--------------+
0x6004'0000 |  firmware    |
            |              |
            |              |
            +--------------+
0x6010'0000 |  VFS         |
            |              |
            /  ...         /
            |              |
            +--------------+
```
Since HyperFlash memory has 256k sectors the firmware has to start at a sector border.
