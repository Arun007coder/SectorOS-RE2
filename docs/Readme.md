# SectorOS RE2

Second rewrite of the SectorOS project. Written in C and assembly.

## Features

* Fully functional printf and sprintf
* Interrupt and exception handling
* string stdlib
* Vga color changing
* IDT and GDT functional
* Paging
* Dynamic memory allocation
* A basic shell
* Virtual filesystem
* EXT2 filesystem
* Driver for AMD PCnet-FAST III
* Semi-functional ATA DMA driver
* Fully functional ATA PIO driver [will be slow]
* Added driver for VESA graphics
* Added Bios 32 service
* Added Init file support
* Added support for TGA and BMP image files
* Added logdisk  

## TODO

* Implement driver for mouse and other devices
* Implement Tasking
* Implement Networking
* Implement ELF loader
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

## Resources

The resources used in this project are:

* [osdev wiki](https://wiki.osdev.org)
* [osdev forums](https://forum.osdev.org)
* [JamesM's kernel development tutorials](http://www.jamesmolloy.co.uk/tutorial_html/)
* [szhou42/osdev](https://github.com/szhou42/osdev)
* [SectorOS](https://github.com/arun007coder/SectorOS)
