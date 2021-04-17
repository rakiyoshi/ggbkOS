; gogobike-os
; TAB=4

    ORG     0xc200      ; プログラムが読み込まれる場所 (0x8000 + 0x4200)
fin:
    HLT
    JMP     fin
