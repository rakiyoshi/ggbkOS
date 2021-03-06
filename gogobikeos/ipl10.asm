; ggbk-ipl
; TAB=4

CYLS    EQU     10          ; どこまで読み込むか

        ORG     0x7c00      ; プログラムが読み込まれる位置

; FAT12 フロッピーディスクのためのファイルヘッダ

        JMP     entry
        DB      0x90
        DB      "GOGOBIKE"      ; ボリュームラベル
        DW      512             ; 1セクタのサイズ
        DB      1               ; クラスタの大きさ
        DW      1               ; FAT が始まるセクタ
        DB      2               ; FAT の個数
        DW      224             ; ルートディレクトリ領域のサイズ
        DW      2880            ; ドライブのサイズ
        DB      0xf0            ; メディアのタイプ
        DW      9               ; FAT 領域のセクタ長
        DW      18              ; 1トラックのセクタ数
        DW      2               ; ヘッド数
        DD      0               ; パーティションを使わない
        DD      2880            ; ドライブのサイズ

; FAT12/16 におけるオフセット36以降のフィールド
        DB      0x00            ; BS_DrvNum
        DB      0x00            ; BS_Reserved1
        DB      0x29            ; BS_BootSig

        DD      0xffffffff      ; シリアル番号
        DB      "GOGOBIKEOS "   ; ディスク名
        DB      "FAT12   "      ; フォーマットの名前
        TIMES   18 DB 0         ;

; エントリ

entry:
        MOV     AX, 0           ; レジスタ初期化
        MOV     SS, AX
        MOV     SP, 0x7c00
        MOV     DS, AX

; ディスクを読む

        MOV     AX, 0x0820
        MOV     ES, AX
        MOV     CH, 0           ; シリンダ0
        MOV     DH, 0           ; ヘッド0
        MOV     CL, 2           ; セクタ2

readloop:
        MOV     SI, 0           ; 失敗回数のカウンタ

retry:
        MOV     AH, 0x02        ; AH=0x02 : ディスク読み込み
        MOV     AL, 1           ; 1セクタ
        MOV     BX, 0
        MOV     DL, 0x00        ; A ドライブ
        INT     0x13            ; ディスクBIOS呼び出し
        JNC     next            ; エラーが起きなければ fin

        ADD     SI, 1           ; SI に1を足す
        CMP     SI, 5
        JAE     error           ; SI >= 5 で error

        MOV     AH, 0x00        ; システムのリセット
        MOV     DL, 0x00        ; A ドライブ
        INT     0x13            ; ディスクBIOS呼び出し
        JMP     retry

next:
        MOV     AX, ES          ; アドレスを0x200進める
        ADD     AX, 0x0020      ; 0x20 = 512 / 16
        MOV     ES, AX          ; ADD ES, 0x020 ができないので、AX で演算する

        ADD     CL, 1           ; increment CL (sector number)
        CMP     CL, 18          ; CL と 18 を比較
        JBE     readloop        ; CL <= 18 の場合は readloop にジャンプ

; ディスクの裏面
        MOV     CL, 1
        ADD     DH, 1
        CMP     DH, 2
        JB      readloop        ; DH < 2 の場合は readloop にジャンプ

; next cylinder
        MOV     DH, 0
        ADD     CH, 1
        CMP     CH, CYLS
        JB      readloop        ; CH < CYLS の場合は readloop にジャンプ

; 読み終わったので haribote.sys を実行する

        MOV     [0x0ff0], CH    ; IPL がどこまで読んだのかをメモ
        JMP     0xc200

error:
        MOV     SI, msg

putloop:
        MOV     AL, [SI]
        ADD     SI, 1
        CMP     AL, 0
        JE      fin

        MOV     AH, 0x0e        ; 一文字表示関数
        MOV     BX, 15          ; カラーコード
        INT     0x10            ; ビデオ BIOS 呼び出し
        JMP     putloop

fin:
        HLT
        JMP     fin             ; 無限ループ

msg:
        DB      0x0a, 0x0a      ; 改行2つ
        DB      "load error"
        DB      0x0a            ; 改行
        DB      0

        TIMES    0x7dfe-0x7c00-($-$$) DB 0  ; 0x7dfe まで 0x00 で埋める

        DB      0x55, 0xaa
