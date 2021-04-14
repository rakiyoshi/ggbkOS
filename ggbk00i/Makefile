WORKDIR=/work

OS_NAME	:= gogobikeos
ASMHEAD	:= asmhead
IPL		:= ipl10

# File names
IMG := $(OS_NAME).img

# Containers
DOCKER		:= docker
DOCKERFLAGS	:= --rm -v $(PWD):$(WORKDIR) -w $(WORKDIR)
NASM_CONTAINER		:= nasm
MTOOLS_CONTAINER	:= mtools
GCC_CONTAINER		:= gcc:10.3.0

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
$(IPL).bin: $(IPL).nas Makefile
	$(NASM) \
		$< \
		-o $@ \
		-l $(IPL).lst

$(ASMHEAD).sys: $(ASMHEAD).nas Makefile
	$(NASM) \
		$< \
		-o $@ \
		-l $(ASMHEAD).lst

$(OS_NAME).img: $(IPL).bin $(ASMHEAD).sys Makefile
	$(MFORMAT) \
		-f 1440 \
		-C \
		-l "GOGOBIKEOS" \
		-B $< \
		-i $@ \
		"::"
	$(MCOPY) \
		-i $@ \
		$(ASMHEAD).sys ::


# Commands
asm: $(IPL).bin

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
