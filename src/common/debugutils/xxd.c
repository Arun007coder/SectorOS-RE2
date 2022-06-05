#include "xxd.h"

void xxd(void* ptr, uint32_t size)
{
    uint32_t i;
    uint32_t j;

    //serial_printf("memaddr | Hexdump                                        | Decoded text\n");

    for(i = 0; i < size + ((size % DUMP_COLS) ? (DUMP_COLS - size % DUMP_COLS) : 0); i++) 
    {
        if(i % DUMP_COLS == 0) 
        {
            serial_printf("0x%06x: ", i);
        }

        if(i < size) 
        {
            serial_printf("%02x ", 0xFF & ((char*)ptr)[i]);
        }
        else
        {
            serial_printf("   ");
        }

        if(i % DUMP_COLS == (DUMP_COLS - 1))
        {
            for(j = i - (DUMP_COLS - 1); j <= i; j++)
            {
                if(j >= size)
                {
                    serial_printf(" ");
                }
                else if(isprint(((char*)ptr)[j]))
                {
                    serial_printf("%c", (((char*)ptr)[j]) & 0xFF);
                }
                else
                {
                    serial_printf(".");
                }
            }
            serial_printf("\n");
        }
    }
}