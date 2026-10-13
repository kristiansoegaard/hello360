# hello360

A small Xbox 360 homebrew demo written in C++ with [libxenon](https://github.com/Free60Project/libxenon). It shows a text console and live controller input (sticks, triggers, buttons, D-pad).

## Requirements
- A Linux machine (or WSL) with the libxenon toolchain installed
- An Xbox 360 that can run unsigned code (RGH/JTAG) with XeLL
- A wired Xbox 360 controller

## Build
```sh
export DEVKITXENON=/usr/local/xenon
make
```
This produces `hello360.elf32`.

## Run
1. Copy `hello360.elf32` to a FAT32 USB drive as `xenon.elf` (or the filename your XeLL setup expects).
2. Boot the console into XeLL.
3. Press **Back + Start** to reboot.

You can also load the file over the network with TFTP.

## Project layout
| File | Purpose |
|------|---------|
| `main.cpp` | App source |
| `Makefile` | Build script for the libxenon toolchain |

## Notes
- If your libxenon install ships a template Makefile that differs from this one, use that and keep `main.cpp`.
- Header and struct names follow libxenon's `input/input.h`; check your install if something fails to compile.

## License
MIT. See `LICENSE`.
