/* 他のファイルで作った関数があることをCコンパイラに伝える */
void io_hlt(void);
void write_mem8(int addr, int data);

/*
 * 関数宣言なのに、{} がなくていきなり ; を書くと、
 * 他のファイルを読むようになる
 */

void HariMain(void)
{
    int i;

    for (i = 0xa0000; i <= 0xaffff; i++) {
        write_mem8(i, 15);
    }

    for (;;) {
        io_hlt();
    }
}
