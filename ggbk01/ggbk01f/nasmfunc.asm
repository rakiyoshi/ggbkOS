; nasmfunc
; TAB=4

section .text
        GLOBAL  io_hlt
        GLOBAL  write_mem8

io_hlt:                     ; void io_hlt(void);
        HLT
        RET

write_mem8:                 ; void write_mem8(int addr, int data);
        MOV     ECX,[ESP+4] ; [ESP+4] に入っている addr を ECX に読み込む
        MOV     AL,[ESP+8]  ; [ESP+8] に入っている data を AL に読み込む
        MOV     [ECX],AL
        RET
