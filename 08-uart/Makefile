##########################################################################
# Raspberry Bare Metal
# Copyright (C) 2014-2015 Federico "MrModd" Cosentino (http://mrmodd.it/)
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
##########################################################################

ifeq ($(CROSS_COMPILE),)
  $(error You need to export CROSS_COMPILE variable first)
endif

CC=$(CROSS_COMPILE)gcc
AS=$(CROSS_COMPILE)as
LD=$(CROSS_COMPILE)ld
NM=$(CROSS_COMPILE)nm

OBJCOPY=$(CROSS_COMPILE)objcopy
OBJDUMP=$(CROSS_COMPILE)objdump
# Alternatively: -O2 || -O3 -fno-tree-vectorize
CFLAGS=-Wall -Wextra -O2 -ffreestanding
ARCHFLAGS=-mcpu=arm1176jzf-s -march=armv6zk -mfpu=vfp -mfloat-abi=hard
CCARCHFLAGS=$(ARCHFLAGS) -marm
CFILES:=$(shell ls *.c 2>/dev/null)
SFILES:=$(shell ls *.S 2>/dev/null)
HFILES:=$(shell ls *.h 2>/dev/null)
AOBJS:=$(SFILES:%.S=%.o)
COBJS:=$(CFILES:%.c=%.o)
TARGET=sert

TAG_SUBDIRS := .
FIND := find
FINDFLAGS := -L

DFLAGS:=



all: $(TARGET).bin $(TARGET).lst $(TARGET).sym

# .bin file is extracted from the .elf. It contains just the binary code
%.bin: %.elf
	$(OBJCOPY) -S -O binary $< $@

# Generate a compiled program with directive for an eventual scheduler that must
# know what to load in RAM and where
$(TARGET).elf: $(AOBJS) $(COBJS) $(TARGET).lds
	$(LD) -nostdlib -T $(TARGET).lds -o $@ $(AOBJS) $(COBJS)

$(COBJS): $(HFILES)

# Generate objects file from assembly sources
%.o: %.S
	$(AS) $(ARCHFLAGS) -o $@ $<

# Generate objects file from C sources
%.o: %.c 
	$(CC) $(CFLAGS) $(CCARCHFLAGS) $(DFLAGS) -c $<

# Generate assembly code from C sources
%.s: %.c
	$(CC) $(CFLAGS) $(CCARCHFLAGS) $(DFLAGS) -S $<

# Generate a dump file of the disassembled binary code
%.lst: %.elf
	$(OBJDUMP) -d $^ > $@

# Generate a list of symbols used in the program
%.sym: %.elf
	$(NM) $^ | sort > $@

# Generate tag file
tags:
	ctags -w -o tags `$(FIND) $(FINDFLAGS) $(TAG_SUBDIRS) -name '*.[chS]' -print`

mini_uart: DFLAGS+=-D MINI_UART
mini_uart: all

# Don't delete these files if make get killed
.PRECIOUS: %.elf

# These rules don't generate files, execute them every time without checking latest changes
.PHONY: clean

clean:
	rm -f *~ *.o *.s *.bin *.elf *.lst *.sym tags

