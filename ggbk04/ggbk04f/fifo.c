/*
 * FIFO Library
 */

#include "bootpack.h"

#define FLAGS_OVERRUN       0x0001


/*
 * FIFO バッファの初期化
 */
void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf)
{
    fifo->size = size;
    fifo->buf = buf;
    fifo->free = size; // 空き
    fifo->flags = 0;
    fifo->p = 0; // 書き込み位置
    fifo->q = 0; // 読み込み位置

    return;
}

/*
 * FIFO へデータを送り込んで蓄える
 */
int fifo8_put(struct FIFO8 *fifo, unsigned char data)
{
    if (fifo->free == 0) {
        fifo-> flags |= FLAGS_OVERRUN; // 空きがなくて溢れた場合
        return -1;
    }
    fifo->buf[fifo->p++] = data;
    if (fifo->p == fifo->size) {
        fifo->p = 0;
    }
    fifo->free--;

    return 0;
}

/*
 * FIFO からデータを一つ取得する
 */
int fifo8_get(struct FIFO8 *fifo)
{
    int data;
    if (fifo->free == fifo->size) {
        return -1; // バッファが空のときは -1 を返す
    }
    data = fifo->buf[fifo->q++];
    if (fifo->q == fifo->size) {
        fifo->q = 0;
    }
    fifo->free++;

    return data;
}

/*
 * データが溜まっている量を取得する
 */
int fifo8_status(struct FIFO8 *fifo)
{
    return fifo->size - fifo->free;
}
