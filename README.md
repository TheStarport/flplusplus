# flplusplus

flplusplus is a client-side hook for Freelancer providing the following features:

- Timestamps for FLSpew.txt
- Removed MP3 codec error spam
- Configurable LOD/Render distance (flplusplus.ini)
- Configurable save game location
- PNG screenshots

The following plugins by adoxa are also included:

- Wheel Scroll

flplusplus also provides the following .ini extensions:

### TTF Loading

In your `FONTS/fonts.ini` file you can create the following section.

```
[FontFiles]
path = FONTS/font.ttf
path = FONTS/font2.ttf
```

This will load the files `FONTS/font.ttf` and `FONTS/font2.ttf` into the process, allowing them to be used by Freelancer without installing them on the system.

## Building

Build on linux using build_linux.sh (CMake 3.13 and mingw-w64 required)

Build on Windows using Visual Studio with CMake

## Thanks

Special thanks to:
- Jason "Adoxa" Hood
- Cannon
- Laz
