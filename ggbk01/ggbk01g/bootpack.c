/* 他のファイルで作った関数があることをCコンパイラに伝える */
extern void io_hlt(void);
extern void io_cli(void);
extern void io_out8(int port, int data);
extern int io_load_eflags(void);
extern void io_store_eflags(int eflags);

void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfil8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);

#define COL8_000000 0
#define COL8_FF0000 1
#define COL8_00FF00 2
#define COL8_FFFF00 3
#define COL8_0000FF 4
#define COL8_FF00FF 5
#define COL8_00FFFF 6
#define COL8_FFFFFF 7
#define COL8_C6C6C6 8
#define COL8_840000 9
#define COL8_008400 10
#define COL8_848400 11
#define COL8_000084 12
#define COL8_840084 13
#define COL8_008484 14
#define COL8_848484 15

void HariMain(void)
{
    unsigned char *p;

    init_palette(); // パレットを設定

    p = (unsigned char *) 0xa0000;

    boxfil8(p, 320, COL8_FF0000,  20,  20, 120, 120);
    boxfil8(p, 320, COL8_00FF00,  70,  50, 170, 150);
    boxfil8(p, 320, COL8_0000FF, 120,  80, 220, 180);

    for (;;) {
        io_hlt();
    }
}

void init_palette(void)
{
    unsigned char table_rgb[16*3] = {
        0x00, 0x00, 0x00, //  0: 黒
        0xff, 0x00, 0x00, //  1: 明るい赤
        0x00, 0xff, 0x00, //  2: 明るい緑
        0xff, 0xff, 0x00, //  3: 明るい黄色
        0x00, 0x00, 0xff, //  4: 明るい青
        0xff, 0x00, 0xff, //  5: 明るい紫
        0x00, 0xff, 0xff, //  6: 明るい水色
        0xff, 0xff, 0xff, //  7: 白
        0xc6, 0xc6, 0xc6, //  8: 明るい灰色
        0x84, 0x00, 0x00, //  9: 暗い赤
        0x00, 0x84, 0x00, // 10: 暗い緑
        0x84, 0x84, 0x00, // 11: 暗い黄色
        0x00, 0x00, 0x84, // 12: 暗い青
        0x84, 0x00, 0x84, // 13: 暗い紫
        0x00, 0x84, 0x84, // 14: 暗い水色
        0x84, 0x84, 0x84  // 15: 暗い灰色
    };
    set_palette(0, 15, table_rgb);
    return;
}

void set_palette(int start, int end, unsigned char *rgb)
{
    int i, eflags;
    eflags = io_load_eflags(); // 割り込み許可フラグの値を取得
    io_cli(); // 許可フラグを0にして割り込み禁止にする

    // [ビデオDAコンバータ](http://oswiki.osask.jp/?VGA#o2d4bfd3)
    io_out8(0x03c8, start); // アドレスレジスタ (パレット書き込みモード)
    for (i = start; i <= end; i++) {
        io_out8(0x03c9, rgb[0] / 4); // データレジスタ
        io_out8(0x03c9, rgb[1] / 4);
        io_out8(0x03c9, rgb[2] / 4);
        rgb += 3;
    }
    io_store_eflags(eflags); // 割り込み許可フラグを元に戻す
    return;
}

void boxfil8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1)
{
    int x, y;
    for (y = y0; y <= y1; y++) {
        for (x = x0; x <= x1; x++) {
            vram[y * xsize + x] = c;
        }
    }
    return;
}
