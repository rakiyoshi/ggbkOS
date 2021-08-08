#include "bootpack.h"

struct MOUSE_DEC {
    unsigned char buf[3], phase;
};

extern struct FIFO8 keyfifo, mousefifo;
void enable_mouse(struct MOUSE_DEC *mdec);
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);
void init_keyboard(void);

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

    enable_mouse(&mdec);

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
                mysprintf(s, "%02X %02X %02X", mdec.buf[0], mdec.buf[1], mdec.buf[2]);
                boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 32, 16, 32+8*8-1, 31);
                putfonts8_asc(binfo->vram, binfo->scrnx, 32, 16, COL8_FFFFFF, s);
            }
        }
    }
}

#define PORT_KEYDAT             0x0060
#define PORT_KEYSTA             0x0064
#define PORT_KEYCMD             0x0064
#define KEYSTA_SEND_NOTREADY    0x02
#define KEYCMD_WRITE_MODE       0x60
#define KBC_MODE                0x47

/*
 * キーボードコントローラがデータ送信可能になるまで待つ
 * KBC: KeyBoard Controller
 */
void wait_KBC_sendready(void)
{
    for (;;) {
        if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
            break;
        }
    }
    return;
}

/*
 * キーボードコントローラの初期化
 */
void init_keyboard(void)
{
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
    wait_KBC_sendready();
    // http://oswiki.osask.jp/?cmd=read&page=%28AT%29keyboard&word=keyboard
    io_out8(PORT_KEYDAT, KBC_MODE);
    return;
}

#define KEYCMD_SENDTO_MOUSE     0xd4
#define MOUSECMD_ENABLE         0xf4

/*
 * マウスの有効化
 */
void enable_mouse(struct MOUSE_DEC *mdec)
{
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
    wait_KBC_sendready();
    io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);  // 成功すると ACK(0xfa) が送信される
    mdec->phase = 0;    // マウスの 0xfa を待っている段階
    return;
}

int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat)
{
    switch (mdec->phase) {
        case 0: // マウスの 0xfa を待っている段階
            if (dat == 0xfa) {
                mdec->phase = 1;
            }
            return 0;
        case 1: // マウスの1バイト目を待っている段階
            mdec->buf[0] = dat;
            mdec->phase++;
            return 0;
        case 2: // マウスの2バイト目を待っている段階
            mdec->buf[1] = dat;
            mdec->phase++;
            return 0;
        case 3: // マウスの3バイト目を待っている段階
            mdec->buf[2] = dat;
            mdec->phase = 1;
            return 1;
        default:
            return -1;
    }
}
