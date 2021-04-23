/* 他のファイルで作った関数があることをCコンパイラに伝える */
void io_hlt(void);

void HariMain(void)
{
    unsigned long int i;
    char *p;

    for (i = 0xa0000; i <= 0xaffff; i++) {
        p = (char *)i;
        *p = i & 0x0f;
    }

    for (;;) {
        io_hlt();
    }
}
