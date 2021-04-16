; naskfunc
; TAB=4

[FORMAT "WCOFF"]            ; オブジェクトファイルを作るモード
[BITS 32]                   ; 32ビットモード用の機械語を作る

; オブジェクトファイルのための情報

[FILE "naskfunc.nas"]       ; ソースファイル名情報

        GLOBAL  _io_hlt     ; このプログラムに含まれる

; 以下、実際の関数

[SECTION .text]             ; オブジェクトファイルではこれを書いてからプログラムを書く

_io_hlt:                    ; void io_hlt(void);
        HLT
        RET
