#ifndef __BLEND_H__
#define __BLEND_H__

#include "system.h"

#define GET_ALPHA(color) ((color >> 24) & 0x000000FF)
#define GET_RED(color)   ((color >> 16)   & 0x000000FF)
#define GET_GREEN(color) ((color >> 8)  & 0x000000FF)
#define GET_BLUE(color)  ((color >> 0)   & 0X000000FF)

#define rgb(r, g, b) (0x00ff0000 & (r << 16)) | (0x0000ff00 & (g << 8)) | (0x000000ff & b)
#define argb(a, r, g, b) (0xff000000 & (a << 16)) | (0x00ff0000 & (r << 16)) | (0x0000ff00 & (g << 8)) | (0x000000ff & b)

#define VESA_COLOR_BLACK rgb(0,0,0)
#define VESA_COLOR_WHITE rgb(255, 255, 255)
#define VESA_COLOR_RED   rgb(255, 0, 0)
#define VESA_COLOR_GREEN rgb(0, 255, 0)
#define VESA_COLOR_BLUE  rgb(0, 0, 255)
#define VESA_COLOR_MAGENTA  rgb(255, 0, 255)

#define SET_ALPHA(color, alpha) ( ((color << 8) >> 8) | ((alpha << 24) & 0xff000000))

uint32_t blend_colors(uint32_t color1, uint32_t color2);

#endif