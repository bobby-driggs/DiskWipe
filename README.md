# DiskWipe
Win32 Disk Wipe Utility.

**Currently a work in progress.**

## How To

Right now everything is hard coded. Update your drive letter in the main.cpp file to the drive letter you want wiped.

```

LPWSTR driveToOpen = L"\\\\.\\G:";   // <--- change drive letter 

```

If you run the program it will start wiping the disk with 0x33333333 for every 4 bytes. It will convert the disk to RAW.

