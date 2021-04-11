; gogobike-os
; TAB=4

    ORG     0xc200      ; プログラムが読み込まれる場所 (0x8000 + 0x4200)

    MOV     AL,0x13     ; VGA グラフィックス(320x200x8bitカラー)
    MOV     AH,0x00
    INT     0x10
fin:
    HLT
    JMP     fin
