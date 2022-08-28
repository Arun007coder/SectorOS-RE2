#include "font.h"
#include "printf.h"

uint32_t font_color = VESA_COLOR_BLACK + 1;

uint32_t get_font_color()
{
    return font_color;
}

void set_font_color(uint32_t color)
{
    font_color = color | 0xff000000;
}

void draw_text(canvas_t * canvas, char * text, int start_x, int start_y)
{
    uint32_t i, x, y, col, row, stop;
    uint8_t *fnt, chr;

    fnt = get_font();
    int len = strlen(text);

    col = start_y;
    row = start_x;
    stop = 0;

    for(i = 0; i < len; i++)
    {
        switch(text[i])
        {
            case '\n':
                row++;
                col = 0;
                chr = 0;
                break;

            case '\r':
                chr = 0;
                break;

            case '\t':
                chr = 0;
                col += 4 - col % 4;
                break;

            case '\0':
                stop = 1;
                chr = 0;
                break;

            default:
                col++;
                chr = text[i];
                break;
        }

        if(stop)
        {
            break;
        }

        if(chr != 0 && canvas->width != 0)
        {
            for(y = 0; y < FONT_HEIGHT; y++)
            {
                for(x = 0; x < FONT_WIDTH; x++)
                {
                    if(fnt[text[i] * FONT_HEIGHT + y] >> (7 - x) & 1)
                    {
                        canvas->framebuffer[((col - 1) * FONT_WIDTH) + x + (y + row * FONT_HEIGHT) * canvas->width] = font_color;
                    }
                }
            }
        }
    }
}

uint32_t row = 0;
uint32_t col = 0;

void veputchar(canvas_t* canvas, char c)
{
    uint32_t i, x, y, stop;
    uint8_t *fnt, chr;

    fnt = get_font();
    int len = 1;

    uint32_t end_x = canvas->width / FONT_WIDTH;
    uint32_t end_y = canvas->height / FONT_HEIGHT;

    stop = 0;

    for(i = 0; i < len; i++)
    {
        switch(c)
        {
            case '\n':
                row++;
                col = 0;
                chr = 0;
                break;

            case '\r':
                chr = 0;
                break;

            case '\t':
                chr = 0;
                col += 4 - col % 4;
                break;

            case '\0':
                stop = 1;
                chr = 0;
                break;

            case '\f':
                row = 0;
                col = 0;
                chr = 0;
                break;

            default:
                col++;
                chr = c;
                break;
        }

        if(stop)
        {
            break;
        }

        if(col > end_x)
        {
            col = 1;
            row++;
        }

        if(row > end_y)
        {
            col = 0;
            row = 0;
        }

        if(chr != 0 && canvas->width != 0)
        {
            for(y = 0; y < FONT_HEIGHT; y++)
            {
                for(x = 0; x < FONT_WIDTH; x++)
                {
                    if(fnt[c * FONT_HEIGHT + y] >> (7 - x) & 1)
                    {
                        canvas->framebuffer[((col - 1) * FONT_WIDTH) + x + (y + row * FONT_HEIGHT) * canvas->width] = font_color;
                    }
                }
            }
        }
    }
}

void veputs(canvas_t* canvas, char* str)
{
    while(*str != 0)
    {
        veputchar(canvas, *str);
        str++;
    }
}

void vevprintf(canvas_t* canvas, char* fmt, va_list arg)
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

    char* str = NULL;

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
                        (*veputchar)(canvas, '-');
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
                        veputchar(canvas, *t);
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
                    (*veputchar)(canvas, (char)va_arg(arg, int));
                }
            }break;
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
                        veputchar(canvas, *t);
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
            (*veputchar)(canvas, c);
        }
    }
}

void veprintf(canvas_t* canvas, const char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    vevprintf(canvas, fmt, arg);
    va_end(arg);
}