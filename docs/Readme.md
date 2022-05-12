# SectorOS RE2

Second rewrite of the SectorOS project. Written in C and assembly.

## Features

* Fully functional printf and sprintf
* Interrupt and exception handling
* string stdlib
* Vga color changing
* IDT and GDT functional

## TODO

* Implement driver for serial port, keyboard, mouse and other devices
* Implement VFS
* Implement memory management
* Implement paging
* etc...

## Build

To build the project, run the following command:
```bash
make -f src/Makefile
```

To build an ISO image, run the following command:
```bash
make -f src/Makefile iso
```

To run the project, run the following command:
```bash
make -f src/Makefile run
```