/*
 * 割り込み関係
 */

#include "bootpack.h"

#define PORT_KEYDAT     0x0060

void init_pic(void)
{
    io_out8(PIC0_IMR, 0xff);    // 全ての割り込みを受け付けない
    io_out8(PIC1_IMR, 0xff);    // 全ての割り込みを受け付けない

    io_out8(PIC0_ICW1, 0x11);   // エッジトリガーモード
    io_out8(PIC0_ICW2, 0x20);   // IRQ0-7 は INT20-27 で受ける
    io_out8(PIC0_ICW3, 1 << 2); // PIC1 は IRQ2 で接続
    io_out8(PIC0_ICW4, 0x01);   // ノンバッファモード

    io_out8(PIC1_ICW1, 0x11);   // エッジトリガモード
    io_out8(PIC1_ICW2, 0x28);   // IRQ8-15 は INT28-2f で受ける
    io_out8(PIC1_ICW3, 2);      // PIC1 は IRQ2 で接続
    io_out8(PIC1_ICW4, 0x01);   // ノンバッファモード

    io_out8(PIC0_IMR, 0xfb);    // 11111011 PIC1 以外は全て禁止
    io_out8(PIC1_IMR, 0xff);    // 11111111 全ての割り込みを受け付けない

    return;
}

struct FIFO8 keyfifo;

/*
 * PS/2 キーボードからの割り込み
 */
void inthandler21(int *esp)
{
    unsigned char data;
    io_out8(PIC0_OCW2, 0x61); // IRQ-01 受付完了を PIC に通知
    data = io_in8(PORT_KEYDAT);
    fifo8_put(&keyfifo, data);
    return;
}

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