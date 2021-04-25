#include "bootpack.h"

void HariMain(void)
{
    struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;
    char s[40], mcursor[256];
    int mx, my;

    init_gdtidt();
    init_pic();
    io_sti();

    init_palette();
    init_screen8(binfo->vram, binfo->scrnx, binfo->scrny);

    // 文字描画
    putfonts8_asc(binfo->vram, binfo->scrnx,  8,  8, COL8_FFFFFF, "Fuck You.");
    putfonts8_asc(binfo->vram, binfo->scrnx, 31, 31, COL8_000000, "GOGOBIKE OS.");
    putfonts8_asc(binfo->vram, binfo->scrnx, 30, 30, COL8_FFFFFF, "GOGOBIKE OS.");

    // マウスカーソル描画
    mx = (binfo->scrnx - 16) / 2;
    my = (binfo->scrny - 28 - 16) / 2;
    init_mouse_cursor8(mcursor, COL8_008484);
    putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
    mysprintf(s, "scrnx = %d", binfo->scrnx);
    putfonts8_asc(binfo->vram, binfo->scrnx, 16, 64, COL8_FFFFFF, s);

    io_out8(PIC0_IMR, 0xf9);    // PIC1 とキーボードを許可 (11111001)
    io_out8(PIC1_IMR, 0xef);    // マウスを許可 (11101111)

    for (;;) {
        io_hlt();
    }
}
