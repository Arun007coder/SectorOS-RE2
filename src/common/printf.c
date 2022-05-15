#include "printf.h"

int is_fmt_letter(char c)
{
    return c == 'c' || c == 'd' || c == 'i' || c == 'e' || c == 'E' || c == 'f' || c == 'g' || c == 'G' || c == 'o' || c == 's' || c == 'u' || c == 'x' || c == 'X' || c == 'p' || c == 'n';
}

void vsprintf(char *str, void (*putchar)(char), const char *fmt, va_list arg)
{
    static uint32_t* pos;
    char c;
    int sign, ival, sys;
    char buf[512];
    char width_str[10];
    uint32_t uval;
    uint32_t size = 8;
    uint32_t i;
    int size_override = 0;
    memset(buf, 0, 512);

    while ((c = *fmt++) != 0)
    {
        sign = 0;

        if (c == '%')
        {
            c = *fmt++;
            switch (c)
            {
            case '0':
                size_override = 1;
                i = 0;
                c = *fmt;
                while (!is_fmt_letter(c))
                {
                    width_str[i++] = c;
                    fmt++;
                    c = *fmt;
                }
                width_str[i] = 0;
                fmt++;
                size = atoi(width_str);
            case 'd':
            case 'u':
            case 'x':
            case 'p':
            {
                if (c == 'd' || c == 'u')
                    sys = 10;
                else
                    sys = 16;
                uval = ival = va_arg(arg, int);
                if (c == 'd' && ival < 0)
                {
                    sign = 1;
                    uval = -ival;
                }
                itoa(buf, uval, sys);
                uint32_t len = strlen(buf);
                if (!size_override)
                    size = len;
                if ((c == 'x' || c == 'p' || c == 'd') && len < size)
                {
                    for (i = 0; i < len; i++)
                    {
                        buf[size - 1 - i] = buf[len - 1 - i];
                    }
                    for (i = 0; i < size - len; i++)
                    {
                        buf[i] = '0';
                    }
                }
                if (c == 'd' && sign)
                {
                    if (str)
                    {
                        *(str + *pos) = '-';
                        *pos = *pos + 1;
                    }
                    else
                        (*putchar)('-');
                }
                if (str)
                {
                    strcpy(str + *pos, buf);
                    *pos = *pos + strlen(buf);
                }
                else
                {
                    char *t = buf;
                    while (*t)
                    {
                        putchar(*t);
                        t++;
                    }
                }
            }
                break;
            case 'c':
            {
                if (str)
                {
                    *(str + *pos) = (char)va_arg(arg, int);
                    *pos = *pos + 1;
                }
                else
                {
                    (*putchar)((char)va_arg(arg, int));
                }
            }
                break;
            case 's':
            {
                if (str)
                {
                    char *t = (char *)va_arg(arg, int);
                    strcpy(str + (*pos), t);
                    *pos = *pos + strlen(t);
                }
                else
                {
                    char *t = (char *)va_arg(arg, int);
                    while (*t)
                    {
                        putchar(*t);
                        t++;
                    }
                }
            }
                break;
            default:
                break;
            }
            continue;
        }
        if (str)
        {
            *(str + *pos) = c;
            *pos = *pos + 1;
        }
        else
        {
            (*putchar)(c);
        }
    }
}

void printf(const char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    vsprintf(NULL, putchar, fmt, arg);
    va_end(arg);
}

void printE(const char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    change_color(VGA_RED, VGA_BLACK);
    vsprintf(NULL, putchar, fmt, arg);
    change_color(VGA_LIGHT_GREEN, VGA_BLACK);
    va_end(arg);
}