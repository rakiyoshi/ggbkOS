#include <stdint.h>
#include "bootpack.h"

unsigned int memtest(unsigned int start, unsigned int end);
unsigned int memtest_sub(unsigned int start, unsigned int end);

void HariMain(void)
{
    struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;
    char s[40], mcursor[256];
    unsigned char keybuf[32], mousebuf[128];
    int mx, my, i;
    struct MOUSE_DEC mdec;

    init_gdtidt();
    init_pic();
    io_sti();

    fifo8_init(&keyfifo, 32, keybuf);
    fifo8_init(&mousefifo, 128, mousebuf);
    io_out8(PIC0_IMR, 0xf9);    // PIC1 とキーボードを許可 (11111001)
    io_out8(PIC1_IMR, 0xef);    // マウスを許可 (11101111)

    init_keyboard();
    enable_mouse(&mdec);

    init_palette();
    init_screen8(binfo->vram, binfo->scrnx, binfo->scrny);

    // 文字描画
    putfonts8_asc(binfo->vram, binfo->scrnx,  8, 60, COL8_FFFFFF, "Fuck You.");
    putfonts8_asc(binfo->vram, binfo->scrnx, 31, 31, COL8_000000, "GOGOBIKE OS.");
    putfonts8_asc(binfo->vram, binfo->scrnx, 30, 30, COL8_FFFFFF, "GOGOBIKE OS.");

    // マウスカーソル描画
    mx = (binfo->scrnx - 16) / 2;
    my = (binfo->scrny - 28 - 16) / 2;
    init_mouse_cursor8(mcursor, COL8_008484);
    putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
    mysprintf(s, "(%d, %d)", mx, my);
    putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);

    i = memtest(0x00400000, 0xbfffffff) / (1024*1024);
    mysprintf(s, "memory %dMB", i);
    putfonts8_asc(binfo->vram, binfo->scrnx, 0, 90, COL8_FFFFFF, s);

    for (;;) {
        io_cli();
        if (fifo8_status(&keyfifo) + fifo8_status(&mousefifo) == 0) {
            io_stihlt();
        } else if (fifo8_status(&keyfifo) != 0) {
            i = fifo8_get(&keyfifo);
            io_sti();
            mysprintf(s, "%02X", i);
            boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
            putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
        } else if (fifo8_status(&mousefifo) != 0) {
            i = fifo8_get(&mousefifo);
            io_sti();
            if (mouse_decode(&mdec, i) != 0) {
                // データが3バイト揃ったので表示
                mysprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
                if ((mdec.btn & 0x1) != 0) {
                    s[1] = 'L';
                }
                if ((mdec.btn & 0x2) != 0) {
                    s[3] = 'R';
                }
                if ((mdec.btn & 0x4) != 0) {
                    s[2] = 'C';
                }
                boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 32, 16, 32+15*8-1, 31);
                putfonts8_asc(binfo->vram, binfo->scrnx, 32, 16, COL8_FFFFFF, s);
                // マウスカーソルの移動
                boxfill8(binfo->vram, binfo->scrnx, COL8_008484, mx, my, mx+15, my+15); // マウスを消す
                mx += mdec.x;
                my += mdec.y;
                if (mx < 0) {
                    mx = 0;
                }
                if (my < 0) {
                    my = 0;
                }
                if (mx > binfo->scrnx - 16) {
                    mx = binfo->scrnx - 16;
                }
                if (my > binfo->scrny - 16) {
                    my = binfo->scrny - 16;
                }
                mysprintf(s, "(%3d, %3d)", mx, my);
                boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 0, 79, 15);  // 座標を消す
                putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s); // 座標を書く
                putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);    // マウスを描く
            }
        }
    }
}

#define EFLAGS_AC_BIT       0x00040000
#define CR0_CACHE_DISABLE   0x60000000

unsigned int memtest(unsigned int start, unsigned int end)
{
    char flg486 = 0;
    unsigned int eflg, cr0, i;

    // Check wether 386 or 486
    eflg = io_load_eflags();
    eflg |= EFLAGS_AC_BIT;  // AC-bit = 1
    io_store_eflags(eflg);
    eflg = io_load_eflags();
    if ((eflg & EFLAGS_AC_BIT) != 0) {  // 386 では AC=1 にしても自動に 0 に戻る
        flg486 = 1;
    }
    eflg &= ~EFLAGS_AC_BIT;    // AC-bit = 0
    io_store_eflags(eflg);

    if (flg486 != 0) {
        cr0 = load_cr0();
        cr0 |= CR0_CACHE_DISABLE;   // キャッシュ禁止
        store_cr0(cr0);
    }

    i = memtest_sub(start, end);

    if (flg486 != 0) {
        cr0 = load_cr0();
        cr0 &= ~CR0_CACHE_DISABLE;  // キャッシュ許可
        store_cr0(cr0);
    }

    return i;
}

unsigned int memtest_sub(unsigned int start, unsigned int end)
{
    unsigned int i, *p, old, pat0 = 0xaa55aa55, pat1 = 0x55aa55aa;

    for (i = start; i <= end; i += 0x1000) {
        p = (unsigned int *) (intptr_t) (i + 0xffc);
        old = *p;           // 変更する前の値を記憶する
        *p = pat0;          // 書き込みをトライする
        *p ^= 0xffffffff;   // 反転する
        if (*p != pat1) {   // 反転したかどうか
not_memory:
            *p = old;
            break;
        }
        *p ^= 0xffffffff;   // もう一度反転する
        if (*p != pat0) {   // 元に戻ったか確認
            goto not_memory;
        }
        *p = old;           // 変更した値を元に戻す
    }

    return i;
}
