WORKDIR=/work

RUNS_ON = default

OS_NAME		:= gogobikeos
ASMHEAD		:= asmhead
IPL			:= ipl10
BOOTPACK	:= bootpack
NASMFUNC	:= nasmfunc
LINKER		:= har
FONT		:= hankaku
FONTCONVERTER	:= hankaku_conv

OBJS_BOOTPACK := $(BOOTPACK).o $(FONT).o nasmfunc.o \
				graphic.o dsctbl.o mysprintf.o int.o fifo.o keyboard.o mouse.o

# Containers
DOCKER		:= docker
DOCKERFLAGS	:= --rm -v $(PWD):$(WORKDIR) -w $(WORKDIR)
NASM_CONTAINER		:= nasm
MTOOLS_CONTAINER	:= mtools
GCC_CONTAINER		:= gcc
RUBY_CONTAINER		:= ruby

# Shell Commands
ifeq (${RUNS_ON}, default)
	NASM	:= $(DOCKER) run $(DOCKERFLAGS) $(NASM_CONTAINER) nasm
	MFORMAT := $(DOCKER) run $(DOCKERFLAGS) $(MTOOLS_CONTAINER) mformat
	MCOPY 	:= $(DOCKER) run $(DOCKERFLAGS) $(MTOOLS_CONTAINER) mcopy
	GCC 	:= $(DOCKER) run $(DOCKERFLAGS) $(GCC_CONTAINER) gcc
	RUBY	:= $(DOCKER) run $(DOCKERFLAGS) $(RUBY_CONTAINER) ruby
else ifeq (${RUNS_ON}, github-actions)
	NASM	:= nasm
	MFORMAT := mformat
	MCOPY 	:= mcopy
	GCC 	:= gcc
	RUBY	:= ruby
endif

MAKE 	:= make -r
RM 		:= rm -f
QEMU 	:= qemu-system-i386


# Default
default:
	$(MAKE) build


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
		-f elf \
		$< \
		-o $@ \
		-l $(NASMFUNC).lst

$(FONT).c: $(FONTCONVERTER).rb $(FONT).txt Makefile
	$(RUBY) \
		$< \
		$(FONT).txt \
		-o $(FONT).c


# コンパイル/リンク
#   gcc:
#     -c:		 リンカを起動しない
#     -march:    システムプロセッサアーキテクチャを指定
#     -m32:	     IA-32 アーキテクチャ向けにコンパイル
#     -nostdlib: 指定されたライブラリだけがリンカに渡される
#     -T:        リンカスクリプトを指定

# コンパイル
%.o: %.c Makefile
	$(GCC) \
		-c \
		-march=i486 \
		-m32 \
		$*.c \
		-o $@
	chmod -x $@

# リンク
$(BOOTPACK).hrb: $(OBJS_BOOTPACK) $(LINKER).ld Makefile
	$(GCC) \
		-march=i486 \
		-m32 \
		-nostdlib \
		-T $(LINKER).ld \
		$(OBJS_BOOTPACK) \
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
		::
	$(MCOPY) \
		-i $@ \
		$(OS_NAME).sys ::


# Commands
build: $(OS_NAME).img

run: $(OS_NAME).img
	$(QEMU) \
		-drive file=$<,format=raw,if=floppy \
		-vnc :0

clean:
	-$(RM) *.bin
	-$(RM) *.lst
	-$(RM) *.sys
	-$(RM) *.img
	-$(RM) *.o
	-$(RM) *.hrb
	-$(RM) hankaku.c
