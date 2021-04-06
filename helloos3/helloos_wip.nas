    ORG     0x7c00          ; nasm にプログラムが読み込まれる番地を指定
; FAT12 フォーマットフロッピーディスク
    JMP     entry
    DB      0x90
    DB      "HELLOIPL"      ; ブートセクタの名前
    DW      512             ; 1セクタの大きさ
    DB      1               ; クラスタの大きさ
    DW      1               ; FAT の開始位置
    DB      2               ; FAT の個数
    DW      224             ; ルートディレクトリ領域の大きさ
    DW      2880
