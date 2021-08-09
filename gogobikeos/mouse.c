#include "bootpack.h"

#define KEYCMD_SENDTO_MOUSE     0xd4
#define MOUSECMD_ENABLE         0xf4

struct FIFO8 mousefifo;

/*
 * PS/2 マウスからの割り込み
 */
void inthandler2c(int *esp)
{
    unsigned char data;
    io_out8(PIC1_OCW2, 0x64);   // IRQ-12 受付完了を PIC1 に通知
    io_out8(PIC0_OCW2, 0x62);   // IRQ-02 受付完了を PIC0 に通知
    data = io_in8(PORT_KEYDAT);
    fifo8_put(&mousefifo, data);
    return;
}

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

/*
 * マウス入力の解読
 */
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat)
{
    switch (mdec->phase) {
        case 0: // マウスの 0xfa を待っている段階
            if (dat == 0xfa) {
                mdec->phase = 1;
            }
            return 0;
        case 1: // マウスの1バイト目を待っている段階
            if ((dat & 0xc8) == 0x08) { // 正しい1バイト目だった場合
                mdec->buf[0] = dat;
                mdec->phase++;
            }
            return 0;
        case 2: // マウスの2バイト目を待っている段階
            mdec->buf[1] = dat;
            mdec->phase++;
            return 0;
        case 3: // マウスの3バイト目を待っている段階
            mdec->buf[2] = dat;
            mdec->phase = 1;
            mdec->btn = mdec->buf[0] & 0x07;    // 下位3ビットを取り出す
            mdec->x = mdec->buf[1];
            mdec->y = mdec->buf[2];
            if ((mdec->buf[0] & 0x10) != 0) {
                mdec->x |= 0xffffff00;
            }
            if ((mdec->buf[0] & 0x20) != 0) {
                mdec->y |= 0xffffff00;
            }
            mdec->y = - mdec->y;
            return 1;
        default:    // ここに来ることは無いはず
            return -1;
    }
}
