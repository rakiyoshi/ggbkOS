/* 他のファイルで作った関数があることをCコンパイラに伝える */
void io_hlt(void);

/*
 * 関数宣言なのに、{} がなくていきなり ; を書くと、
 * 他のファイルを読むようになる
 */

void HariMain(void)
{

fin:
    io_hlt();
    goto fin;
}
