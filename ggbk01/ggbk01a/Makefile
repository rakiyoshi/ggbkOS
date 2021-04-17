WORKDIR=/work

OS_NAME		:= gogobikeos
ASMHEAD		:= asmhead
IPL			:= ipl10
BOOTPACK	:= bootpack
NASMFUNC	:= nasmfunc
LINKER		:= har

# File names
IMG := $(OS_NAME).img

# Containers
DOCKER		:= docker
DOCKERFLAGS	:= --rm -v $(PWD):$(WORKDIR) -w $(WORKDIR)
NASM_CONTAINER		:= nasm
MTOOLS_CONTAINER	:= mtools
GCC_CONTAINER		:= gcc

# Shell Commands
NASM	:= $(DOCKER) run $(DOCKERFLAGS) $(NASM_CONTAINER) nasm
MFORMAT := $(DOCKER) run $(DOCKERFLAGS) $(MTOOLS_CONTAINER) mformat
MCOPY 	:= $(DOCKER) run $(DOCKERFLAGS) $(MTOOLS_CONTAINER) mcopy
GCC 	:= $(DOCKER) run $(DOCKERFLAGS) $(GCC_CONTAINER) gcc
MAKE 	:= make -r
RM 		:= rm -f
QEMU 	:= qemu-system-i386


# Default
default:
	$(MAKE) img


# File generation
$(IPL).bin: $(IPL).asm Makefile
	$(NASM) \
		$< \
		-o $@ \
		-l $(IPL).lst

$(ASMHEAD).bin: $(ASMHEAD).asm Makefile
	$(NASM) \
		$< \
		-o $@ \
		-l $(ASMHEAD).lst

$(NASMFUNC).o: $(NASMFUNC).asm Makefile
	$(NASM) \
		-g \
		-f elf \
		$< \
		-o $@

$(BOOTPACK).hrb: $(BOOTPACK).c $(LINKER).ld $(NASMFUNC).o Makefile	# コンパイル/リンク
	$(GCC) \
		-march=i486 \
		-m32 \
		-nostdlib \
		-T $(LINKER).ld \
		-g $(BOOTPACK).c $(NASMFUNC).o \
		-o $@
	chmod -x $@

$(OS_NAME).sys: $(ASMHEAD).bin $(BOOTPACK).hrb
	cat $^ > $@

$(OS_NAME).img: $(IPL).bin $(OS_NAME).sys Makefile
	$(MFORMAT) \
		-f 1440 \
		-C \
		-l "GOGOBIKEOS" \
		-B $< \
		-i $@ \
		"::"
	$(MCOPY) \
		-i $@ \
		$(OS_NAME).sys ::


# Commands
img: $(OS_NAME).img

run: $(OS_NAME).img
	$(QEMU) \
		-drive file=$<,format=raw,if=floppy \
		-vnc :0

clean:
	-$(RM) *.bin
	-$(RM) *.lst
	-$(RM) *.sys
	-$(RM) *.img
	-$(RM) *.hrb
	-$(RM) *.o
