; gogobike-os
; TAB=4

; BOOT_INFO
CYLS    EQU     0x0ff0          ; ブートセクタが設定する
LEDS    EQU     0x0ff1
VMODE   EQU     0x0ff2          ; 色数に関する情報, カラービット数
SCRNX   EQU     0x0ff4          ; 解像度の X
SCRNY   EQU     0x0ff6          ; 解像度の Y
VRAM    EQU     0x0ff8          ; グラッフィクバッファの開始番地

        ORG     0xc200          ; プログラムが読み込まれる場所 (0x8000 + 0x4200)

        MOV     AL,0x13         ; VGA グラフィックス(320x200x8bitカラー)
        MOV     AH,0x00
        INT     0x10
        MOV     BYTE [VMODE],8  ; 画面モードをメモする
        MOV     WORD [SCRNX],320
        MOV     WORD [SCRNY],200
        MOV     DWORD [VRAM],0x000a0000

; キーボードのLED状態を BIOS から取得する

        MOV     AH,0x02
        INT     0x16            ; Keyboard BIOS
        MOV     [LEDS],AL

fin:
    HLT
    JMP     fin
