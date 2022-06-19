#ifndef __INITFILE_H__
#define __INITFILE_H__

#define INITFILE_PATH "/init"

#include "system.h"
#include "string.h"
#include "list.h"
#include "vfs.h"
#include "printf.h"
#include "vesa.h"
#include "bitmap.h"
#include "targa.h"
#include "ext2.h"

void load_initfile();

#endif