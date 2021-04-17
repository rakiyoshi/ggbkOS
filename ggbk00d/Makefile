WORKDIR=/work

# File names
IMAGE_NAME := ggbkos
BOOTSECTOR_NAME := ipl

# Containers
NASM_CONTAINER := nasm
MTOOLS_CONTAINER := mtools

# Shell Commands
NASM := docker run --rm -v $(PWD):$(WORKDIR) $(NASM_CONTAINER) nasm
MFORMAT := docker run --rm -v $(PWD):$(WORKDIR) $(MTOOLS_CONTAINER) mformat
MAKE := make -r
RM := rm -f
QEMU := qemu-system-x86_64


# Default
default:
	$(MAKE) img

# File generation
$(BOOTSECTOR_NAME).bin: ipl.nas Makefile
	$(NASM) \
		$(WORKDIR)/$(BOOTSECTOR_NAME).nas \
		-o $(WORKDIR)/$(BOOTSECTOR_NAME).bin \
		-l $(WORKDIR)/$(BOOTSECTOR_NAME).lst

$(IMAGE_NAME).img: ipl.bin ipl.lst Makefile
	$(MFORMAT) \
		-f 1440 \
		-C \
		-l "HELLO-OS" \
		-B $(WORKDIR)/$(BOOTSECTOR_NAME).bin \
		-i $(WORKDIR)/$(IMAGE_NAME).img \
		::

# Commands
asm: $(BOOTSECTOR_NAME).bin

img: $(IMAGE_NAME).img

run: $(IMAGE_NAME).img
	$(QEMU) \
		-drive file=$(IMAGE_NAME).img,format=raw,if=floppy \
		-nographic

clean:
	-$(RM) $(BOOTSECTOR_NAME).bin
	-$(RM) $(BOOTSECTOR_NAME).lst

src_only:
	$(MAKE) clean
	-$(RM) $(IMAGE_NAME).img
