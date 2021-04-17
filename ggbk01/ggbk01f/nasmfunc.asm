; nasmfunc
; TAB=4

section .text
        GLOBAL  io_hlt
        GLOBAL  write_mem8

io_hlt:                     ; void io_hlt(void);
        HLT
        RET
