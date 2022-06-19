PREFIX=/opt/cross/bin/

shell= /bin/sh
date= $(shell date)

RESET= \x1B[0m
ULN  = \x1B[21m
BLK  = \x1B[30m
RED  = \x1B[31m
GRN  = \x1B[32m
YEL  = \x1B[33m
BLU  = \x1B[34m
MAR  = \x1B[35m
CYN  = \x1B[36m
WHT  = \x1B[37m

CC = i686-elf-gcc
INC = -Isrc/Include/ -I/usr/include
GCCVERSION = $(shell echo $(CC) & $(PREFIX)$(CC) --version | grep ^gcc | sed 's/^.* //g')
CFLAGS = $(INC) -DKERNEL_BUILD="\"$(date)\"" -DCOMPILER="\"$(GCCVERSION)\"" -nostdlib -fno-builtin -fno-exceptions -fno-leading-underscore -Wno-write-strings -Waddress-of-packed-member 
CDFLAGS = -g

LD = i686-elf-ld
LINKERSCRIPT = src/link.ld
LDFLAGS = -T$(LINKERSCRIPT)

AS = i686-elf-as
ASFLAGS =

ASM = nasm
ASMFLAGS =-f elf32 -O0 -w+zeroing

GRUBDIR = ~/local/bin/
SRCDIR = src/

OBJCOPY = i686-elf-objcopy
OBJDUMP = i686-elf-objdump

OBJCOPYFLAGS = --strip-debug --strip-unneeded

PROJECT = SectorOS_RE2
EXECUTABLE = $(PROJECT).elf
ISOIMAGE = $(PROJECT).iso

HDIMAGE = ext2_hda.img

QEMU = qemu-system-i386
QEMUFLAGS = -kernel $(EXECUTABLE) -m 1024 -drive file=$(HDIMAGE),media=disk,index=0,format=raw
QEMUDFLAGS = -serial file:serial.log -s -S -daemonize

OBJECTS = \
	$(SRCDIR)multiboot.o \
	$(SRCDIR)loader.o	\
	$(SRCDIR)descriptors/gdt_helper.o	\
	$(SRCDIR)descriptors/gdt.o	\
	$(SRCDIR)drivers/cpu/pic.o	\
	$(SRCDIR)drivers/cpu/pit.o	\
	$(SRCDIR)drivers/sound/beeper.o	\
	$(SRCDIR)Interrupt/exception_helper.o	\
	$(SRCDIR)Interrupt/exception.o	\
	$(SRCDIR)Interrupt/interrupt_helper.o	\
	$(SRCDIR)Interrupt/interrupt.o	\
	$(SRCDIR)Interrupt/syscall.o	\
	$(SRCDIR)descriptors/idt_helper.o	\
	$(SRCDIR)descriptors/idt.o	\
	$(SRCDIR)hardcom/port.o \
	$(SRCDIR)hardcom/mmio.o \
	$(SRCDIR)drivers/pci.o \
	$(SRCDIR)mem/pmm.o	\
	$(SRCDIR)mem/kheap.o	\
	$(SRCDIR)mem/paging.o	\
	$(SRCDIR)drivers/time/rtc.o \
	$(SRCDIR)common/bios32.o \
	$(SRCDIR)common/bios32_helper.o \
	$(SRCDIR)drivers/input/keyboard.o	\
	$(SRCDIR)drivers/serial/serial.o \
	$(SRCDIR)drivers/video/vga.o \
	$(SRCDIR)drivers/video/vesa.o \
	$(SRCDIR)drivers/storage/ata.o \
	$(SRCDIR)drivers/storage/ata_pio.o \
	$(SRCDIR)gui/bitmap.o \
	$(SRCDIR)gui/targa.o \
	$(SRCDIR)common/fast_memcpy.o \
	$(SRCDIR)common/getEIP.o \
	$(SRCDIR)common/string.o \
	$(SRCDIR)drivers/storage/logdisk.o \
	$(SRCDIR)common/printf.o \
	$(SRCDIR)common/system.o \
	$(SRCDIR)common/cpuid.o \
	$(SRCDIR)common/structures/list.o \
	$(SRCDIR)common/structures/gentree.o \
	$(SRCDIR)drivers/ethernet/am79c973.o \
	$(SRCDIR)common/debugutils/xxd.o \
	$(SRCDIR)fs/vfs.o \
	$(SRCDIR)fs/devfs.o \
	$(SRCDIR)fs/ext2.o \
	$(SRCDIR)fs/procfs.o \
	$(SRCDIR)program/shell.o \
	$(SRCDIR)program/initfile.o \
	$(SRCDIR)Kernel/KernelMain.o \

all: $(ISOIMAGE)

$(EXECUTABLE): $(OBJECTS)
	@printf '$(YEL)[LD] Linking object files...$(RESET)\n'
	@$(PREFIX)$(LD) $(LDFLAGS) -o $@ $(OBJECTS)
	@printf '$(BLU)Kernel surcessfully builded. kernel can be found in $(shell pwd)/$(EXECUTABLE)...\n$(RESET)'

kernel: $(EXECUTABLE)

$(ISOIMAGE): stripd $(EXECUTABLE)
	@printf '$(CYN)[GRUB] Building ISO image...$(RESET)\n'
	@mkdir -p $(PROJECT)/boot/grub
	@cp $(EXECUTABLE) $(PROJECT)/boot/
	@echo 'set timeout=3' >> $(PROJECT)/boot/grub/grub.cfg
	@echo 'set default=0' >> $(PROJECT)/boot/grub/grub.cfg
	@echo '' >> $(PROJECT)/boot/grub/grub.cfg
	@echo 'menuentry "SectorOS" { '>> $(PROJECT)/boot/grub/grub.cfg
	@echo 'multiboot /boot/$(EXECUTABLE)' >> $(PROJECT)/boot/grub/grub.cfg
	@echo 'boot' >> $(PROJECT)/boot/grub/grub.cfg
	@echo '}' >> $(PROJECT)/boot/grub/grub.cfg
	@$(GRUBDIR)/grub-mkrescue -o $(ISOIMAGE) $(PROJECT)
	@rm -rf $(PROJECT)/
	@printf '$(BLU)ISO Image surcessfully builded. image can be found in $(shell pwd)/$(ISOIMAGE)...\n$(RESET)'

iso: $(ISOIMAGE)

run: $(ISOIMAGE) $(HDIMAGE) addfiles
	@printf '$(BLU)Running $(ISOIMAGE) using $(QEMU)...\n$(RESET)'
	@$(QEMU) $(QEMUFLAGS) -serial stdio 2> /dev/null

run_kvm: $(ISOIMAGE) $(HDIMAGE) addfiles
	@printf '$(BLU)Running $(ISOIMAGE) using $(QEMU)...\n$(RESET)'
	@$(QEMU) $(QEMUFLAGS) -serial stdio -enable-kvm 2> /dev/null

rund: $(ISOIMAGE) $(HDIMAGE) addfiles
	@printf '$(MAR)Running $(ISOIMAGE) using $(QEMU) with debug enabled...\n$(RESET)'
	@$(QEMU) $(QEMUFLAGS) $(QEMUDFLAGS)

runBochs: $(ISOIMAGE) $(HDIMAGE) addfiles
	@printf '$(BLU)Running $(ISOIMAGE) using Bochs...\n$(RESET)'
	bochs -f docs/bochsrc

addfiles: $(HDIMAGE) nbdmount
	@printf '$(BLU)Adding files to $(HDIMAGE)...\n$(RESET)'
	@sudo mkdir -p /mnt/dev
	@sudo cp files/* /mnt/
	@printf '$(BLU)Files added to $(HDIMAGE)...\n$(RESET)'
	@make -f src/Makefile nbdumount

force: clean run
forced: clean rund
forceBochs: clean runBochs
forceBochsd: clean rund
forceKVM: clean run_kvm

$(HDIMAGE):
	@printf '$(BLU)Making and formatting $(HDIMAGE)...\n$(RESET)'
	@dd if=/dev/zero of=ext2_hda.img bs=1k count=100000 2> /dev/null
	@mkfs -t ext2 -i 1024 -b 1024 -F ext2_hda.img 2> /dev/null
	@$(shell fdisk ext2_hda.img << 'EOF' 2> /dev/null \
	x \
	c \
	10 \
	h \
	16 \
	s \
	63 \
	r \
	n \
	p \
	a \
	w \
	EOF)

mkimg: $(HDIMAGE)
delimg: 
	@printf '$(RED)Deleting $(HDIMAGE)...\n$(RESET)'
	@rm -f ext2_hda.img

nbdmount: $(HDIMAGE)
	@printf '$(BLU)Mounting $(HDIMAGE) using nbd...\n$(RESET)'
	sudo modprobe nbd
	sleep 1
	udevadm settle
	sudo qemu-nbd -c /dev/nbd0 $(HDIMAGE) --format raw
	udevadm settle
	sudo mount /dev/nbd0 /mnt

nbdumount: $(HDIMAGE)
	@printf '$(BLU)Unmounting $(HDIMAGE) using nbd...\n$(RESET)'
	sudo umount /mnt
	udevadm settle
	sudo qemu-nbd -d /dev/nbd0
	udevadm settle
	sudo modprobe -r nbd

stripd: $(EXECUTABLE)
	@printf '$(BLU)[OBJCOPY] Copying debug symbols to debug.sym...\n$(RESET)'
	@$(PREFIX)$(OBJCOPY) --only-keep-debug $(EXECUTABLE) debug.sym
	@printf '$(BLU)[OBJCOPY] Stripping $(EXECUTABLE)...\n$(RESET)'
	@$(PREFIX)$(OBJCOPY) $(OBJCOPYFLAGS) $(EXECUTABLE)

PHONY: lines
lines:
	@echo -e '$(BLU)lines of code in the project:$(RESET)'
	@find -iregex '.*\.\(c\|asm\|s\|h\)$$' | xargs wc -l

%.o: %.c
	@printf '$(GRN)[CC] $@$(RESET)\n'
	@$(PREFIX)$(CC) $(CFLAGS) $(CDFLAGS) -c -o $@ $<

%.o: %.s
	@printf '$(GRN)[GAS] $@$(RESET)\n'
	@$(PREFIX)$(AS) $(ASFLAGS) -o $@ $<

%.o: %.asm
	@printf '$(GRN)[NASM] $@$(RESET)\n'
	@$(ASM) $(ASMFLAGS) -o $@ $<

PHONY: clean
clean:
	@printf '$(RED)Cleaning object files, kernel and iso image...\n$(RESET)'
	@rm -rf $(OBJECTS) $(EXECUTABLE) $(ISOIMAGE) debug.sym

install_prereqs:
	@printf '$(BLU)Installing prerequisites...\n$(RESET)'
	@wget https://raw.githubusercontent.com/lordmilko/i686-elf-tools/master/i686-elf-tools.sh
	@chmod +x i686-elf-tools.sh
	@./i686-elf-tools.sh linux