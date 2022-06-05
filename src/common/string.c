#include "string.h"
#include "printf.h"
#include "system.h"
#include "kheap.h"

#define FAST_MEMCPY 0

void fast_memcpy(char * dst, const char * src, uint32_t n);

int memcmp(uint8_t *data1, uint8_t *data2, int n)
{
    while (n--)
    {
        if (*data1 != *data2)
            return 0;
        data1++;
        data2++;
    }
    return 1;
}

void *memcpy(void *dst, void const *src, int n)
{
#if FAST_MEMCPY
    fast_memcpy(dst, src, n);
    return dst;
#else
    char *ret = (char*)dst;
    char *p = (char*)dst;
    const char *q = (char*)src;
    while (n--)
        *p++ = *q++;
    return ret;
#endif
}

void *memset(void *dst, char val, int n)
{
    char *temp = (char*)dst;
    for (; n != 0; n--)
        *temp++ = val;
    return dst;
}

uint16_t *memsetw(uint16_t *dest, uint16_t val, uint32_t count)
{
    uint16_t *temp = (uint16_t *)dest;
    for (; count != 0; count--)
        *temp++ = val;
    return dest;
}

uint16_t *memsetdw(uint32_t *dest, uint32_t val, uint32_t count)
{
    uint32_t *temp = (uint32_t *)dest;
    for (; count != 0; count--)
        *temp++ = val;
    return (uint16_t*)dest;
}

int strlen(const char *s)
{
    int len = 0;
    while (*s++)
        len++;
    return len;
}
char *strncpy(char *destString, const char *sourceString, int maxLength)
{
    unsigned count;

    if ((destString == (char *)NULL) || (sourceString == (char *)NULL))
    {
        return (destString = NULL);
    }

    if (maxLength > 255)
        maxLength = 255;

    for (count = 0; (int)count < (int)maxLength; count++)
    {
        destString[count] = sourceString[count];

        if (sourceString[count] == '\0')
            break;
    }

    if (count >= 255)
    {
        return (destString = NULL);
    }

    return (destString);
}

int strcmp(const char *dst, char *src)
{
    int i = 0;

    while ((dst[i] == src[i]))
    {
        if (src[i++] == 0)
            return 0;
    }
    return 1;
}

char *strstr(const char *in, const char *str)
{
    char c;
    uint32_t len;

    c = *str++;
    if (!c)
        return (char *)in;

    len = strlen(str);
    do
    {
        char sc;

        do
        {
            sc = *in++;
            if (!sc)
                return (char *)0;
        } while (sc != c);
    } while (strncmp(in, str, len) != 0);

    return (char *)(in - 1);
}

int strcpy(char *dst, const char *src)
{
    int i = 0;
    while ((*dst++ = *src++) != 0)
        i++;
    return i;
}

void strcat(void *dest, const void *src)
{
    char *end = (char *)dest + strlen((char*)dest);
    memcpy((char *)end, (char *)src, strlen((char *)src));
    end = end + strlen((char *)src);
    *end = '\0';
}

int strncmp(const char *s1, const char *s2, int c)
{
    int result = 0;

    while (c)
    {
        result = *s1 - *s2++;

        if ((result != 0) || (*s1++ == 0))
        {
            break;
        }

        c--;
    }

    return result;
}

void itoa(char *buf, unsigned long int n, int base)
{
    unsigned long int tmp;
    int i, j;

    tmp = n;
    i = 0;

    do
    {
        tmp = n % base;
        buf[i++] = (tmp < 10) ? (tmp + '0') : (tmp + 'a' - 10);
    } while (n /= base);
    buf[i--] = 0;

    for (j = 0; j < i; j++, i--)
    {
        tmp = buf[j];
        buf[j] = buf[i];
        buf[i] = tmp;
    }
}

int atoi(char *string)
{
    int result = 0;
    unsigned int digit;
    int sign;

    while (isspace(*string))
    {
        string += 1;
    }

    if (*string == '-')
    {
        sign = 1;
        string += 1;
    }
    else
    {
        sign = 0;
        if (*string == '+')
        {
            string += 1;
        }
    }

    for (;; string += 1)
    {
        digit = *string - '0';
        if (digit > 9)
        {
            break;
        }
        result = (10 * result) + digit;
    }

    if (sign)
    {
        return -result;
    }
    return result;
}


char* itoa_r(unsigned long int n, int base)
{
    unsigned long int tmp;
    int i, j;
    char* buf = (char*)kmalloc(sizeof(char) * 32);

    tmp = n;
    i = 0;

    do
    {
        tmp = n % base;
        buf[i++] = (tmp < 10) ? (tmp + '0') : (tmp + 'a' - 10);
    } while (n /= base);
    buf[i--] = 0;

    for (j = 0; j < i; j++, i--)
    {
        tmp = buf[j];
        buf[j] = buf[i];
        buf[i] = tmp;
    }

    return buf;
}

int isspace(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}

int isprint(char c)
{
    return ((c >= ' ' && c <= '~') ? 1 : 0);
}

char *strdup(const char *src)
{
    int len = strlen(src) + 1;
    char *dst = (char*)kmalloc(len);
    memcpy(dst, src, len);
    return dst;
}

char *strndup(const char *src, uint32_t len)
{
    char *dst = (char*)kmalloc(len + 1);
    memcpy(dst, src, len);
    dst[len] = '\0';
    return dst;
}

char *strsep(char **stringp, const char *delim) {
    char *s;
    const char *spanp;
    int c, sc;
    char *tok;
    if ((s = *stringp) == NULL)
        return (NULL);
    for (tok = s;;) {
        c = *s++;
        spanp = delim;
        do {
            if ((sc = *spanp++) == c) {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                *stringp = s;
                return (tok);
            }
        } while (sc != 0);
    }
}

bool alloc = false;

void stoc(size_t n)
{
    if(((n / MB) >> 10) != 0)
    {
        printf("%dGB", n / GB);
    }
    else if (((n / KB) >> 10) != 0)
    {
        printf("%dMB", n / MB);
    }
    else if (((n) >> 10) != 0)
    {
        printf("%dKB", n / KB);
    }
    else
    {
        printf("%dB", n);
    }
}

list_t *str_split(const char *str, const char *delim, unsigned int *numtokens)
{
    list_t *ret_list = list_create();
    char *s = strdup(str);
    char *token, *rest = s;
    while ((token = strsep(&rest, delim)) != NULL)
    {
        if (!strcmp(token, "."))
            continue;
        if (!strcmp(token, ".."))
        {
            if (list_size(ret_list) > 0)
                list_pop(ret_list);
            continue;
        }
        list_push(ret_list, strdup(token));
        if (numtokens)
            (*numtokens)++;
    }
    kfree(s);
    return ret_list;
}

char *list2str(list_t *list, const char *delim)
{
    char *ret = (char*)kmalloc(256);
    memset(ret, 0, 256);
    int len = 0, ret_len = 256;
    while (list_size(list) > 0)
    {
        char *temp = (char*)list_pop(list)->val;
        int len_temp = strlen(temp);
        if (len + len_temp + 1 + 1 > ret_len)
        {
            ret_len = ret_len * 2;
            ret = (char*)krealloc(ret, ret_len);
            len = len + len_temp + 1;
        }
        strcat(ret, delim);
        strcat(ret, temp);
    }
    return ret;
}


void sprintf(char *s, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsprintf(s, NULL, format, args);
    va_end(args);
}
