WORKDIR=/work

NASM_CONTAINER := nasm
MTOOLS_CONTAINER := mtools
NASM := docker run --rm -v ${PWD}:${WORKDIR} ${NASM_CONTAINER} nasm
MFORMAT := docker run --rm -v ${PWD}:${WORKDIR} ${MTOOLS_CONTAINER} mformat

IMAGE_PATH=helloos.img

ipl.bin: ipl.nas Makefile
	$(NASM) ${WORKDIR}/ipl.nas \
		-o ${WORKDIR}/ipl.bin \
		-l ${WORKDIR}/ipl.lst

helloos.img: ipl.bin ipl.lst Makefile
	$(MFORMAT) -f 1440 \
		-C \
		-l "HELLO-OS" \
		-B ${WORKDIR}/ipl.bin \
		-i ${WORKDIR}/helloos.img \
		::

asm:
	make -r ipl.bin

run:
	make -r img
	qemu-system-x86_64 -drive file=${IMAGE_PATH},format=raw -nographic

clean:
	-rm -f ipl.bin
	-rm -f ipl.lst
	-rm -f helloos.img

img:
	make -r helloos.img
