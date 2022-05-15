PREFIX=/opt/cross/bin/

shell= /bin/sh
date= $(shell date)

CC = i686-elf-gcc
INC = -Isrc/Include/ -I/usr/include
GCCVERSION = $(shell echo $(CC) & $(PREFIX)$(CC) --version | grep ^gcc | sed 's/^.* //g')
CFLAGS = $(INC) -g -DKERNEL_BUILD="\"$(date)\"" -DCOMPILER="\"$(GCCVERSION)\"" -nostdlib -fno-builtin -fno-exceptions -fno-leading-underscore -Wno-write-strings

LD = i686-elf-ld
LINKERSCRIPT = src/link.ld
LDFLAGS = -T$(LINKERSCRIPT)

AS = $(PREFIX)i686-elf-as
ASFLAGS =

ASM = nasm
ASMFLAGS =-f elf32 -O0

GRUBDIR = ~/local/bin/
SRCDIR = src/

PROJECT = SectorOS_RE2
EXECUTABLE = $(PROJECT).elf
ISOIMAGE = $(PROJECT).iso

QEMU = qemu-system-i386
QEMUFLAGS = -boot d -cdrom $(ISOIMAGE) -s -S -daemonize

OBJECTS = \
	$(SRCDIR)multiboot.o \
	$(SRCDIR)loader.o	\
	$(SRCDIR)descriptors/gdt_helper.o	\
	$(SRCDIR)descriptors/gdt.o	\
	$(SRCDIR)drivers/cpu/pic.o	\
	$(SRCDIR)drivers/cpu/pit.o	\
	$(SRCDIR)Interrupt/exception_helper.o	\
	$(SRCDIR)Interrupt/exception.o	\
	$(SRCDIR)Interrupt/interrupt_helper.o	\
	$(SRCDIR)Interrupt/interrupt.o	\
	$(SRCDIR)descriptors/idt_helper.o	\
	$(SRCDIR)descriptors/idt.o	\
	$(SRCDIR)hardcom/port.o \
	$(SRCDIR)drivers/pci.o \
	$(SRCDIR)mem/pmm.o	\
	$(SRCDIR)mem/kheap.o	\
	$(SRCDIR)mem/paging.o	\
	$(SRCDIR)drivers/input/keyboard.o	\
	$(SRCDIR)drivers/serial/serial.o \
	$(SRCDIR)drivers/video/vga.o \
	$(SRCDIR)common/string.o \
	$(SRCDIR)common/printf.o \
	$(SRCDIR)common/system.o \
	$(SRCDIR)common/cpuid.o \
	$(SRCDIR)common/structures/list.o \
	$(SRCDIR)common/structures/gentree.o \
	$(SRCDIR)fs/vfs.o \
	$(SRCDIR)program/shell.o \
	$(SRCDIR)Kernel/KernelMain.o \

all: $(ISOIMAGE)

$(EXECUTABLE): $(OBJECTS)
	$(PREFIX)$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

$(ISOIMAGE): $(EXECUTABLE)
	@mkdir -p $(PROJECT)/boot/grub
	@cp $(EXECUTABLE) $(PROJECT)/boot/
	@echo 'set timeout=3' >> $(PROJECT)/boot/grub/grub.cfg
	@echo 'set default=0' >> $(PROJECT)/boot/grub/grub.cfg
	@echo '' >> $(PROJECT)/boot/grub/grub.cfg
	@echo 'menuentry "SectorOS" { '>> $(PROJECT)/boot/grub/grub.cfg
	@echo 'multiboot /boot/$(EXECUTABLE)' >> $(PROJECT)/boot/grub/grub.cfg
	@echo 'boot' >> $(PROJECT)/boot/grub/grub.cfg
	@echo '}' >> $(PROJECT)/boot/grub/grub.cfg
	$(GRUBDIR)/grub-mkrescue -o $(ISOIMAGE) $(PROJECT)
	@rm -rf $(PROJECT)/

iso: $(ISOIMAGE)

run: $(ISOIMAGE)
	$(QEMU) $(QEMUFLAGS)

force: clean run

%.o: %.c
	$(PREFIX)$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.S
	$(PREFIX)$(AS) $(ASFLAGS) -o $@ $<

%.o: %.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

PHONY: clean
clean:
	@rm -rf $(OBJECTS) $(EXECUTABLE) $(ISOIMAGE)