; gogobike-os
; TAB=4

BOTPAK  EQU     0x00280000      ; bootpack のロード先
DSKCAC  EQU     0x00100000      ; ディスクキャッシュの場所
DSKCAC0 EQU     0x00008000      ; ディスクキャッシュの場所 (リアルモード)

; BOOT_INFO
CYLS    EQU     0x0ff0          ; ブートセクタが設定する
LEDS    EQU     0x0ff1          ; LED STATE
VMODE   EQU     0x0ff2          ; 色数に関する情報, カラービット数
SCRNX   EQU     0x0ff4          ; 解像度の X
SCRNY   EQU     0x0ff6          ; 解像度の Y
VRAM    EQU     0x0ff8          ; グラッフィクバッファの開始番地

        ORG     0xc200          ; プログラムが読み込まれる場所 (0x8000 + 0x4200)
                                ; イメージファイルの 0x4200 番地に書き込まれる
                                ; 先で 0x8000 移行を使う

; [画面モード設定](http://oswiki.osask.jp/?%28AT%29BIOS#n5884802)
;     AL = モード： (メジャーな画面モードのみ)
;         0x03：16色テキスト
;               80x25
;         0x12：VGAグラフィックス
;               640x480x4bitカラー
;               独自プレーンアクセス
;         0x13：VGAグラフィックス
;               320x200x8bitカラー
;               パックドピクセル
;         0x6a：拡張VGAグラフィックス
;               800x600x4bitカラー
;               独自プレーンアクセス（ビデオカードによってはサポートされない）
;     AH = 0x00;
;     戻り値：なし

        MOV     AL,0x13         ; VGA グラフィックス(320x200x8bitカラー)
        MOV     AH,0x00
        INT     0x10

; 画面モードをメモする
        MOV     BYTE [VMODE],8          ; Video Mode
        MOV     WORD [SCRNX],320        ; SCReeN X
        MOV     WORD [SCRNY],200        ; SCReeN Y
        MOV     DWORD [VRAM],0x000a0000 ; Video RAM


; キーボードのLED状態を BIOS から取得する
; [キーロック＆シフト状態取得](http://oswiki.osask.jp/?%28AT%29BIOS#lb9f3e72)
; AH = モード (キーロック＆シフト状態取得)
;     AH = 0x02;
; 戻り値：
;     AL == 状態コード：
;         bit0：右シフト
;         bit1：左シフト
;         bit2：Ctrl
;         bit3：Alt
;         bit4：Scrollロック
;         bit5：Numロック
;         bit6：Capsロック
;         bit7：Insertモード

        MOV     AH,0x02
        INT     0x16            ; Keyboard BIOS
        MOV     [LEDS],AL

; PIC が一切の割り込みを受け付けないようにする
;   AT 互換機の使用では、 PIC の初期化をするなら、
;   こちらは CLI 前にやっておかないと、たまにハングアップする。
;   PIC の初期化はあとでやる

        MOV     AL,0xff
        OUT     0x21,AL
        NOP                     ; OUT 命令を連続させると上手く動作しない場合がある
        OUT     0xa1,AL

        CLI                     ; さらに CPU レベルでも割り込み禁止

; CPU から 1MB 以上のメモリにアクセスできるように A20GATE を設定

        CALL    waitkbdout
        MOV     AL,0xd1
        OUT     0x64,AL
        CALL    waitkbdout
        MOV     AL,0xdf         ; enable A20
        OUT     0x60,AL
        CALL    waitkbdout

; プロテクトモード移行

        LGDT    [GDTR0]         ; 暫定 GDT を設定
        MOV     EAX,CR0
        AND     EAX,0x7fffffff  ; bit31 を 0 にする (ページング禁止のため)
        OR      EAX,0x00000001  ; bit0 を 1 にする (プロテクトモード移行のため)
        MOV     CR0,EAX
        JMP     pipelineflush

pipelineflush:
        MOV     AX,1*8          ; 読み書き可能セグメント 32bit
        MOV     DS,AX
        MOV     ES,AX
        MOV     FS,AX
        MOV     GS,AX
        MOV     SS,AX

; bootpack の転送

        MOV     ESI,bootpack    ; 転送元
        MOV     EDI,BOTPAK      ; 転送先
        MOV     ECX,512*1024/4
        CALL    memcpy

; ディスクデータも本来の位置へ転送

; ブートセクタを転送

        MOV     ESI,0x7c00      ; 転送元
        MOV     EDI,DSKCAC      ; 転送先
        MOV     ECX,512/4
        CALL    memcpy

; 残りを転送

        MOV     ESI,DSKCAC0+512 ; 転送元
        MOV     EDI,DSKCAC+512  ; 転送先
        MOV     ECX,0
        MOV     CL,BYTE [CYLS]
        IMUL    ECX,512*18*2/4  ; シリンダの数からバイト数/4 に変換
        SUB     ECX,512/4       ; IPL の分だけ差し引く
        CALL    memcpy

; asmhead でしなければいけないことは全部完了した
; あとは bootpack に任せる

; bootpack の起動

        MOV     EBX,BOTPAK
        MOV     ECX,[EBX+16]
        ADD     ECX,3           ; ECX += 3
        SHR     ECX,2           ; ECX /= 4
        JZ      skip           ; 転送するべきものがないとき skip へ
        MOV     ESI,[EBX+20]    ; 転送元
        ADD     ESI,EBX
        MOV     EDI,[EBX+12]    ; 転送先
        CALL    memcpy
skip:
        MOV     ESP,[EBX+12]    ; スタック初期化
        JMP     DWORD 2*8:0x0000001b

waitkbdout:
        IN      AL,0x64
        AND     AL,0x02
        ;IN      AL,0x60         ; から読み(受信バッファが悪さをしないように)
        JNZ     waitkbdout      ; AND の結果が0でなければ waitkbdout へ
        RET

memcpy:
        MOV     EAX,[ESI]
        ADD     ESI,4
        MOV     [EDI],EAX
        ADD     EDI,4
        SUB     ECX,1
        JNZ     memcpy          ; 引き算した結果が0でなければ memcpy へ
        RET
; memcpy はアドレスサイズプリフィクスを入れ忘れなければ、ストリング命令でもかける

        ALIGNB  16, DB 0
GDT0:
        TIMES   8 DB 0          ; null selector
        DW      0xffff,0x0000,0x9200,0x00cf ; 読み書き可能セグメント 32bit
        DW      0xffff,0x0000,0x9a28,0x0047 ; 実行可能セグメント 32bit (bootpack 用)

        DW      0
GDTR0:
        DW      8*3-1
        DD      GDT0

        ALIGNB  16, DB 0
bootpack:
