#include <stdarg.h>

struct Padding {
    char str;
    int length;
};

/*
 * 10進数から ASCII コードに変換する
 * 10で割れた回数(桁数)を len に、各桁をbuf に格納
 */
int dec2asc(char *str, int dec, struct Padding padding)
{
    int len = 0, len_buf;
    int buf[10];
    while (1) {
        buf[len++] = dec % 10;
        if (dec < 10) break;
        dec /= 10;
    }
    if (padding.length == 0) {
        len_buf = len;
    } else {
        len_buf = padding.length;
        // padding.str で (padding.len - len) 個パディング
        while (padding.length - len) {
            *(str++) = padding.str;
            padding.length--;
        }
    }
    while (len) {
        *(str++) = buf[--len] + 0x30;
    }
    return len_buf;
}

/*
 * 16進数から ASCII コードに変換
 * 16で割れた回数(桁数)を len に、各桁をbuf に格納
 */
int hex2asc(char *str, int dec, struct Padding padding)
{
    int len = 0, len_buf;
    int buf[10];
    while (1) {
        buf[len++] = dec % 16;
        if (dec < 16) break;
        dec /= 16;
    }
    if (padding.length == 0) {
        len_buf = len;
    } else {
        len_buf = padding.length;
        // padding.str で (padding.len - len) 個パディング
        while (padding.length - len) {
            *(str++) = padding.str;
            padding.length--;
        }
    }
    while (len) {
        len--;
        *(str++) = (buf[len]<10) ? (buf[len]+0x30) : (buf[len]-9+0x60);
    }
    return len_buf;
}

/*
 * 16進数から ASCII コード(アルファベット大文字)に変換
 * 16で割れた回数(桁数)を len に、各桁をbuf に格納
 */
int hex2asc_upper(char *str, int dec, struct Padding padding)
{
    int len = 0, len_buf;
    int buf[10];
    while (1) {
        buf[len++] = dec % 16;
        if (dec < 16) break;
        dec /= 16;
    }
    if (padding.length == 0) {
        len_buf = len;
    } else {
        len_buf = padding.length;
        // padding.str で (padding.len - len) 個パディング
        while (padding.length - len) {
            *(str++) = padding.str;
            padding.length--;
        }
    }
    while (len) {
        len--;
        *(str++) = (buf[len]<10) ? (buf[len]+0x30) : (buf[len]-9+0x40);
    }
    return len_buf;
}


void mysprintf(char *str, char *fmt, ...)
{
    va_list list;
    int i, len;
    struct Padding padding = {'E', 0};
    va_start(list, fmt);

    while (*fmt) {
        if (*fmt=='%') {
            fmt++;
            if (*fmt != 'd' && *fmt != 'D' && *fmt != 'x') {
                // パディング
                // 02x の場合、 0 がパディング文字、2が出力文字列長
                padding.str = *(fmt++);
                padding.length = *(fmt++) - 0x30;
            }
            switch(*fmt) {
                case 'd':
                    len = dec2asc(str, va_arg(list, int), padding);
                    padding.str = 'E';
                    padding.length = 0;
                    break;
                case 'x':
                    len = hex2asc(str, va_arg(list, int), padding);
                    padding.str = 'E';
                    padding.length = 0;
                    break;
                case 'X':
                    len = hex2asc_upper(str, va_arg(list, int), padding);
                    padding.str = 'E';
                    padding.length = 0;
                    break;
            }
            str += len;
            fmt++;
        } else {
            *(str++) = *(fmt++);
        }
    }
    *str = 0x00;
    va_end(list);
}
