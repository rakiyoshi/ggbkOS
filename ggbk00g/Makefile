WORKDIR=/work

OS_NAME := ggbkos
BOOTSECTOR_NAME := ipl

# File names
IMG := $(OS_NAME).img
IMG_SRC  := $(OS_NAME).nas
OSSYS_FILE := $(OS_NAME).sys
OSSYS_SRC  := $(OS_NAME).nas
OSSYS_LST  := $(OS_NAME).lst
BOOTSECTOR_SRC := $(BOOTSECTOR_NAME).nas
BOOTSECTOR_BIN := $(BOOTSECTOR_NAME).bin
BOOTSECTOR_LST := $(BOOTSECTOR_NAME).lst

# Containers
NASM_CONTAINER := nasm
MTOOLS_CONTAINER := mtools

# Shell Commands
NASM	:= docker run --rm -v $(PWD):$(WORKDIR) -w $(WORKDIR) $(NASM_CONTAINER) nasm
MFORMAT := docker run --rm -v $(PWD):$(WORKDIR) -w $(WORKDIR) $(MTOOLS_CONTAINER) mformat
MCOPY 	:= docker run --rm -v $(PWD):$(WORKDIR) -w $(WORKDIR) $(MTOOLS_CONTAINER) mcopy
MAKE 	:= make -r
RM 		:= rm -f
QEMU 	:= qemu-system-x86_64


# Default
default:
	$(MAKE) img


# File generation
$(BOOTSECTOR_BIN): $(BOOTSECTOR_SRC) Makefile
	$(NASM) \
		$< \
		-o $@ \
		-l $(BOOTSECTOR_LST)

$(IMG): $(BOOTSECTOR_BIN) $(BOOTSECTOR_LST) $(OSSYS_FILE) $(OSSYS_LST) Makefile
	$(MFORMAT) \
		-f 1440 \
		-C \
		-l "HELLO-OS" \
		-B $< \
		-i $@ \
		"::"
	$(MCOPY) \
		-i $(IMG) \
		$(OSSYS_FILE) ::

$(OSSYS_FILE): $(OSSYS_SRC) Makefile
	$(NASM) \
		$< \
		-o $@ \
		-l $(OSSYS_LST)


# Commands
asm: $(BOOTSECTOR_BIN)

img: $(IMG)

run: $(IMG)
	$(QEMU) \
		-drive file=$(IMG),format=raw,if=floppy \
		-vnc :0

clean:
	-$(RM) *.bin
	-$(RM) *.lst
	-$(RM) *.sys

src_only:
	$(MAKE) clean
	-$(RM) $(IMG)
