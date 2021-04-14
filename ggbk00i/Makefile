WORKDIR=/work

OS_NAME := gogobikeos
ASMHEAD := asmhead
IPL := ipl10

# File names
IMG := $(OS_NAME).img

# Containers
NASM_CONTAINER := nasm
MTOOLS_CONTAINER := mtools

# Shell Commands
NASM	:= docker run --rm -v $(PWD):$(WORKDIR) -w $(WORKDIR) $(NASM_CONTAINER) nasm
MFORMAT := docker run --rm -v $(PWD):$(WORKDIR) -w $(WORKDIR) $(MTOOLS_CONTAINER) mformat
MCOPY 	:= docker run --rm -v $(PWD):$(WORKDIR) -w $(WORKDIR) $(MTOOLS_CONTAINER) mcopy
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

$(IMG): $(IPL).bin $(IPL).lst $(ASMHEAD).sys $(ASMHEAD).lst Makefile
	$(MFORMAT) \
		-f 1440 \
		-C \
		-l "GOGOBIKEOS" \
		-B $< \
		-i $@ \
		"::"
	$(MCOPY) \
		-i $(IMG) \
		$(ASMHEAD).sys ::


# Commands
asm: $(IPL).bin

img: $(IMG)

run: $(IMG)
	$(QEMU) \
		-drive file=$(IMG),format=raw,if=floppy \
		-vnc :0

clean:
	-$(RM) *.bin
	-$(RM) *.lst
	-$(RM) *.sys
	-$(RM) *.img
