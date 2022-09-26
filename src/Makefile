# Copyright (C) 2022 Arun007coder
# 
# This file is part of SectorOS-RE2.
# 
# SectorOS-RE2 is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# SectorOS-RE2 is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with SectorOS-RE2.  If not, see <http://www.gnu.org/licenses/>.

PREFIX=/opt/cross/bin/

DATE = $(shell date)

shell= /bin/bash

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
CFLAGS = $(INC) -DKERNEL_BUILD="\"$(DATE)\"" -DCOMPILER="\"$(GCCVERSION)\"" -nostdlib -fno-builtin -fno-exceptions -fno-leading-underscore -Wno-write-strings
CDFLAGS = -g

LD = i686-elf-ld
LINKERSCRIPT = src/link.ld
LDFLAGS = -T$(LINKERSCRIPT)

AS = i686-elf-as
ASFLAGS =

ASM = nasm
ASMFLAGS =-f elf32 -O0 -w+zeroing

GRUBDIR = ~/local/bin/
SRCDIR = src
SCRIPTDIR = scripts
BACKUP_FOLDER = ../backup_SectorOS_RE2

OBJCOPY = i686-elf-objcopy
OBJDUMP = i686-elf-objdump

OBJCOPYFLAGS = --strip-debug --strip-unneeded

PROJECT = SectorOS_RE2
EXECUTABLE = $(PROJECT).elf
ISOIMAGE = $(PROJECT).iso

HDIMAGE = ext2_hda.img

QEMU = qemu-system-i386
QEMUFLAGS = -kernel $(EXECUTABLE) -m 2048 -drive file=$(HDIMAGE),media=disk,index=0,format=raw -netdev tap,id=mynet0,ifname=tap0,script=no,downscript=no -device rtl8139,netdev=mynet0,mac=52:55:00:d1:55:01

QEMUDFLAGS = -serial file:serial.log -s -S -daemonize

NETWORK_INTERFACE = enp2s0

OBJECTS = \
	$(SRCDIR)/multiboot.o \
	$(SRCDIR)/loader.o	\
	$(SRCDIR)/process/spinlock.o \
	$(SRCDIR)/descriptors/gdt_helper.o	\
	$(SRCDIR)/descriptors/gdt.o	\
	$(SRCDIR)/drivers/cpu/pic.o	\
	$(SRCDIR)/drivers/cpu/pit.o	\
	$(SRCDIR)/drivers/cpu/apm.o	\
	$(SRCDIR)/drivers/sound/beeper.o	\
	$(SRCDIR)/Interrupt/exception_helper.o	\
	$(SRCDIR)/Interrupt/exception.o	\
	$(SRCDIR)/Interrupt/interrupt_helper.o	\
	$(SRCDIR)/Interrupt/interrupt.o	\
	$(SRCDIR)/Interrupt/syscall.o	\
	$(SRCDIR)/descriptors/idt_helper.o	\
	$(SRCDIR)/descriptors/idt.o	\
	$(SRCDIR)/descriptors/tss.o	\
	$(SRCDIR)/descriptors/tss_helper.o	\
	$(SRCDIR)/hardcom/port.o \
	$(SRCDIR)/hardcom/mmio.o \
	$(SRCDIR)/drivers/pci.o \
	$(SRCDIR)/mem/pmm.o	\
	$(SRCDIR)/mem/kheap.o	\
	$(SRCDIR)/mem/paging.o	\
	$(SRCDIR)/drivers/time/rtc.o \
	$(SRCDIR)/common/bios32.o \
	$(SRCDIR)/common/math.o \
	$(SRCDIR)/common/bios32_helper.o \
	$(SRCDIR)/drivers/input/keyboard.o	\
	$(SRCDIR)/drivers/serial/serial.o \
	$(SRCDIR)/drivers/video/vga.o \
	$(SRCDIR)/drivers/video/vesa.o \
	$(SRCDIR)/drivers/storage/ata.o \
	$(SRCDIR)/drivers/storage/ata_pio.o \
	$(SRCDIR)/gui/bitmap.o \
	$(SRCDIR)/gui/targa.o \
	$(SRCDIR)/common/fast_memcpy.o \
	$(SRCDIR)/common/getEIP.o \
	$(SRCDIR)/common/string.o \
	$(SRCDIR)/drivers/storage/logdisk.o \
	$(SRCDIR)/drivers/storage/ramdisk.o \
	$(SRCDIR)/drivers/storage/nulldev.o \
	$(SRCDIR)/drivers/storage/randomdev.o \
	$(SRCDIR)/common/printf.o \
	$(SRCDIR)/common/system.o \
	$(SRCDIR)/common/cpuid.o \
	$(SRCDIR)/common/structures/list.o \
	$(SRCDIR)/common/structures/gentree.o \
	$(SRCDIR)/drivers/ethernet/am79c973.o \
	$(SRCDIR)/drivers/ethernet/rtl8139.o \
	$(SRCDIR)/common/debugutils/xxd.o \
	$(SRCDIR)/fs/vfs.o \
	$(SRCDIR)/fs/devfs.o \
	$(SRCDIR)/fs/ext2.o \
	$(SRCDIR)/fs/procfs.o \
	$(SRCDIR)/fs/sorfs.o \
	$(SRCDIR)/fs/mbr.o \
	$(SRCDIR)/fs/mount.o \
	$(SRCDIR)/network/netutils.o \
	$(SRCDIR)/network/netinterface.o \
	$(SRCDIR)/network/ethernet.o \
	$(SRCDIR)/network/arp.o \
	$(SRCDIR)/network/ipv4.o \
	$(SRCDIR)/network/icmp.o \
	$(SRCDIR)/network/udp.o \
	$(SRCDIR)/network/dhcp.o \
	$(SRCDIR)/program/shell.o \
	$(SRCDIR)/program/initfile.o \
	$(SRCDIR)/process/usermode.o \
	$(SRCDIR)/process/process.o \
	$(SRCDIR)/process/context_switch.o \
	$(SRCDIR)/gui/blend.o \
	$(SRCDIR)/gui/draw.o \
	$(SRCDIR)/gui/font/font.o \
	$(SRCDIR)/gui/font/font_parser.o \
	$(SRCDIR)/Kernel/KernelMain.o \

all: $(ISOIMAGE)

$(SRCDIR)/mksorfs:
	/usr/bin/g++ -I$(SRCDIR)//tools/Include -o $(SRCDIR)/mksorfs $(SRCDIR)/tools/mksorfs/mksorfs.cpp

$(EXECUTABLE): $(OBJECTS)
	@printf '$(YEL)[LD] Linking object files...$(RESET)\n'
	@$(PREFIX)$(LD) $(LDFLAGS) -o $@ $(OBJECTS)
	@printf '$(BLU)Kernel surcessfully builded. kernel can be found in $(shell pwd)/$(EXECUTABLE)...\n$(RESET)'

kernel: $(EXECUTABLE)

setupTAP:
	@printf '$(BLU)Setting up tap\n$(RESET)'
	@sudo brctl addbr br0
	@sudo ip addr flush dev $(NETWORK_INTERFACE)
	@sudo brctl addif br0 $(NETWORK_INTERFACE)
	@sudo tunctl -t tap0 -u `whoami`
	@sudo brctl addif br0 tap0
	@sudo ifconfig $(NETWORK_INTERFACE) up
	@sudo ifconfig tap0 up
	@sudo ifconfig br0 up
	@sudo dhclient -v br0

stopTAP:
	@sudo brctl delif br0 tap0
	@sudo tunctl -d tap0
	@sudo brctl delif br0 $(NETWORK_INTERFACE)
	@sudo ifconfig br0 down
	@sudo brctl delbr br0
	@sudo ifconfig $(NETWORK_INTERFACE) up
	@sudo dhclient -v $(NETWORK_INTERFACE)

stopTAP1:
	@sudo ifconfig br0 down
	@sudo brctl delbr br0
	@sudo ifconfig $(NETWORK_INTERFACE) up
	@sudo dhclient -v $(NETWORK_INTERFACE)

wireshark:
	@printf '$(BLU)Starting wireshark\n$(RESET)'
	@sudo wireshark -k -i tap0 &

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
	@sudo $(QEMU) $(QEMUFLAGS) -serial stdio 2> /dev/null

runKVM: $(ISOIMAGE) $(HDIMAGE) addfiles
	@printf '$(BLU)Running $(ISOIMAGE) using $(QEMU)...\n$(RESET)'
	@sudo $(QEMU) $(QEMUFLAGS) -serial stdio -enable-kvm 2> /dev/null

rund: $(ISOIMAGE) $(HDIMAGE) addfiles
	@printf '$(MAR)Running $(ISOIMAGE) using $(QEMU) with debug enabled...\n$(RESET)'
	@sudo $(QEMU) $(QEMUFLAGS) $(QEMUDFLAGS)

runBochs: $(ISOIMAGE) $(HDIMAGE) addfiles
	@printf '$(BLU)Running $(ISOIMAGE) using Bochs...\n$(RESET)'
	bochs -f docs/bochsrc

addfiles: $(HDIMAGE) nbdmount
	@printf '$(BLU)Adding files to $(HDIMAGE)...\n$(RESET)'
	@echo ${PASSWORD} | sudo -S mkdir -p /mnt/dev
	@echo ${PASSWORD} | sudo -S mkdir -p /mnt/proc
	@echo ${PASSWORD} | sudo -S mkdir -p /mnt/mnt
	@./$(SCRIPTDIR)/addfiles.sh
	@printf '$(BLU)Files added to $(HDIMAGE)...\n$(RESET)'
	@make -f src/Makefile nbdumount

force: clean run
forced: clean rund
forceBochs: clean runBochs
forceBochsd: clean rund
forceKVM: clean runKVM

$(HDIMAGE):
	@printf '$(BLU)Making and formatting $(HDIMAGE)...\n$(RESET)'
	@./$(SCRIPTDIR)/mkimg.sh
	@make -f src/Makefile nbdstart
	@echo ${PASSWORD} | sudo -S mkfs -t ext2 -i 1024 -b 1024 -F /dev/nbd0p1 2> /dev/null
	@make -f src/Makefile nbdstop

mkimg: $(HDIMAGE)
delimg: 
	@printf '$(RED)Deleting $(HDIMAGE)...\n$(RESET)'
	@rm -f ext2_hda.img

nbdstart: $(HDIMAGE)
	@printf '$(BLU)Mounting $(HDIMAGE) using nbd...\n$(RESET)'
	echo ${PASSWORD} | sudo -S modprobe nbd
	sleep 1
	udevadm settle
	echo ${PASSWORD} | sudo -S qemu-nbd -c /dev/nbd0 $(HDIMAGE) --format raw
	udevadm settle

nbdmount: nbdstart
	@printf '$(BLU)Mounting $(HDIMAGE) using nbd...\n$(RESET)'
	echo ${PASSWORD} | sudo -S mount /dev/nbd0p1 /mnt

nbdstop:
	echo ${PASSWORD} | sudo -S qemu-nbd -d /dev/nbd0
	echo ${PASSWORD} | sudo -S modprobe -r nbd || true
	sleep 1
	udevadm settle

backup:
	@printf '$(BLU)Backing up the project...\n$(RESET)'
	@mkdir -p $(BACKUP_FOLDER)
	@cp -r * $(BACKUP_FOLDER)

nbdumount: $(HDIMAGE)
	@printf '$(BLU)Unmounting $(HDIMAGE) using nbd...\n$(RESET)'
	echo ${PASSWORD} | sudo -S umount /mnt
	udevadm settle
	echo ${PASSWORD} | sudo -S qemu-nbd -d /dev/nbd0
	udevadm settle
	echo ${PASSWORD} | sudo -S modprobe -r nbd

$(SRCDIR)/initrd: $(SRCDIR)/mksorfs
	./$(SRCDIR)/mksorfs $(SRCDIR)/initrd files/init

mkinitrd: $(SRCDIR)/initrd

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
