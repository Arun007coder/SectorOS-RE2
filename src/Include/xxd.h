#ifndef __XXD_H
#define __XXD_H

#include "system.h"
#include "printf.h"
#include "string.h"
#include "serial.h"

#define DUMP_COLS 16

/*
* @brief A function to dump the contents of a variable in a xxd like style
* @param ptr pointer to the data
* @param size size of the variable
*/
void xxd(void* ptr, uint32_t size);

#endif