; ggbk-ipl
; TAB=4

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
    DB      0,0,0x29        ;
    DD      0xffffffff      ; シリアル番号
    DB      "GOGOBIKEOS "   ; ディスク名
    DB      "FAT12"         ; フォーマットの名前
    TIMES   18 DB 0         ;

; エントリ

entry:
    MOV     AX,0            ; レジスタ初期化
    MOV     SS,AX
    MOV     SP,0x7c00
    MOV     DS,AX

; ディスクを読む

    MOV     AX,0x0820
    MOV     ES,AX
    MOV     CH,0            ; シリンダ0
    MOV     DH,0            ; ヘッド0
    MOV     CL,2            ; セクタ2

    MOV     SI,0            ; 失敗回数のカウンタ

retry:
    MOV     AH,0x02         ; AH=0x02 : ディスク読み込み
    MOV     AL,1            ; 1セクタ
    MOV     BX,0
    MOV     DL,0x00         ; A ドライブ
    INT     0x13            ; ディスクBIOS呼び出し
    JNC     fin             ; エラーが起きなければ fin
    ADD     SI,1            ; SI に1を足す
    CMP     SI,5
    JAE     error           ; SI >= 5 で error
    MOV     AH,0x00         ; システムのリセット
    MOV     DL,0x00         ; A ドライブ
    INT     0x13            ; ディスクBIOS呼び出し
    JMP retry

; 終了ループ

fin:
    HLT                     ; CPU 停止
    JMP     fin             ; 無限ループ

error:
    MOV     AX,0
    MOV     ES,AX
    MOV     SI,msg

putloop:
    MOV     AL,[SI]
    ADD     SI,1
    CMP     AL,0
    JE      fin
    MOV     AH,0x0e         ; 一文字表示関数
    MOV     BX,15           ; カラーコード
    INT     0x10            ; ビデオ BIOS 呼び出し
    JMP     putloop

msg:
    DB      0x0a,0x0a       ; 改行2つ
    DB      "load error"
    DB      0x0a            ; 改行
    DB      0

    TIMES    0x7dfe-0x7c00-($-$$) DB 0  ; 0x7dfe まで 0x00 で埋める

    DB      0x55,0xaa
