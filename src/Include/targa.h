#ifndef __TARGA_H__
#define __TARGA_H__

#include "system.h"
#include "string.h"
#include "vesa.h"

uint32_t* targa_parse(uint8_t* buffer, uint32_t size);
void targa_display(char* name);

#endif