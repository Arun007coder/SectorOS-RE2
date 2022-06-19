#include "vesa.h"

uint32_t current_mode;
int isVesaInitialized = 0;

void vesa_memcpy24_to_32(uint24_t* dest, uint32_t* src, size_t size)
{
    uint24_t t;
    uint32_t i;
    for(i = 0; i < size; i++)
    {
        t.integer = src[i];
        dest[i] = t;
    }
}

void vesa_memsetRGB(uint8_t * dest, uint32_t rgb, uint32_t count)
{
    if(count % 3 != 0)
        count = count + 3 - (count % 3);
    uint8_t r = rgb & 0x00ff0000;
    uint8_t g = rgb & 0x0000ff00;
    uint8_t b = rgb & 0x000000ff;
    for(int i = 0; i < count; i++)
    {
        *dest++ = r;
        *dest++ = g;
        *dest++ = b;
    }
}

void* vesa_getFramebuffer()
{
    VBE_MODE_INFO_t mode_info = {0};
    vesa_getModeInfo(current_mode, &mode_info);
    return (void*)mode_info.phys_base;
}

int vesa_getXResolution()
{
    VBE_MODE_INFO_t mode_info = {0};
    vesa_getModeInfo(current_mode, &mode_info);
    return mode_info.XResolution;
}

int vesa_getYResolution()
{
    VBE_MODE_INFO_t mode_info = {0};
    vesa_getModeInfo(current_mode, &mode_info);
    return mode_info.YResolution;
}

void vesa_getModeInfo(uint32_t mode, VBE_MODE_INFO_t* modeInfo)
{
    register16_t in_reg;
    register16_t out_reg;

    in_reg.ax = 0x4f01;
    in_reg.cx = mode;

    // ES:DI 0000:9000
    in_reg.es = 0x0000;
    in_reg.di = 0x9000;

    bios32_call(BIOS_GRAPHICS_SERVICE, &in_reg, &out_reg);

    if(out_reg.ax != 0x004f)
    {
        printE("Error: VESA call failed!\n");
        return;
    }

    memcpy(modeInfo, (void*)0x9000, sizeof(VBE_MODE_INFO_t));
}

void vesa_setMode(uint32_t mode)
{
    register16_t in_reg;
    register16_t out_reg;

    in_reg.ax = 0x4f02;
    in_reg.bx = mode;

    bios32_call(BIOS_GRAPHICS_SERVICE, &in_reg, &out_reg);

    if(out_reg.ax != 0x004f)
    {
        printE("Error: VESA call failed!\n");
        return;
    }

    current_mode = mode;
}

uint32_t vesa_find_mode(uint32_t width, uint32_t height, uint32_t bpp)
{
    VBE_INFO_t * vbe_info;
    VBE_MODE_INFO_t * mode_info;
    register16_t reg_in = {0};
    register16_t reg_out = {0};

    reg_in.ax = 0x4F00;
    reg_in.di = 0x9500;
    bios32_call(BIOS_GRAPHICS_SERVICE, &reg_in, &reg_out);
    memcpy(vbe_info, (void*)0x9500, sizeof(VBE_INFO_t));

    uint16_t * mode_list = (uint16_t*)vbe_info->video_mode_list;
    uint16_t mode_number = *mode_list++;
    while(mode_number != 0xffff)
    {
        vesa_getModeInfo(mode_number, mode_info);
        if(mode_info->XResolution == width && mode_info->YResolution == height && mode_info->bpp == bpp)
            return mode_number;
        mode_number = *mode_list++;
    }
    return 0;
}

void vesa_putPixel(uint32_t x, uint32_t y, uint32_t color)
{
    VBE_MODE_INFO_t mode_info = {0};
    vesa_getModeInfo(current_mode, &mode_info);

    uint32_t pitch = mode_info.pitch;
    uint32_t bpp = mode_info.bpp;
    uint32_t framebuffer = mode_info.phys_base;

    uint32_t pixel_offset = y * pitch + (x * (bpp/8)) + framebuffer;

    ((uint8_t*)pixel_offset)[0] = color & 0x000000ff;
    ((uint8_t*)pixel_offset)[1] = (color & 0x0000ff00) >> 8;
    ((uint8_t*)pixel_offset)[2] = (color & 0x00ff0000) >> 16;
    ((uint8_t*)pixel_offset)[3] = (color & 0xff000000) >> 24;
}

uint32_t rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return (0x00ff0000 & (r << 16)) | (0x0000ff00 & (g << 8)) | (0x000000ff & b);
}

void vesa_change_mode(uint32_t width, uint32_t height, uint32_t bpp)
{
    uint32_t mode = vesa_find_mode(width, height, bpp);
    if(mode == 0)
    {
        printE("Error: Could not find a suitable mode!\n");
        return;
    }
    vesa_setMode(0x4000 | mode);
}

void init_vesa()
{
    uint32_t mode = vesa_find_mode(1024, 768, 32);
    
    if(mode == 0)
    {
        kernel_panic("Error: Could not find VESA mode!\n");
        return;
    }

    vesa_setMode(0x4000 | mode);

    void* framebuffer = vesa_getFramebuffer();
    alloc_region(kernel_page_dir, (uint32_t)framebuffer, (uint32_t)(framebuffer + vesa_getXResolution() * vesa_getYResolution() * 4), 1, 1, 1);

    isVesaInitialized = 1;

    serial_printf("VESA initialized!\n");
    VBE_MODE_INFO_t mode_info = {0};
    vesa_getModeInfo(current_mode, &mode_info);

    serial_printf("Mode: %x\n", current_mode);
    serial_printf("XResolution: %d\n", mode_info.XResolution);
    serial_printf("YResolution: %d\n", mode_info.YResolution);
    serial_printf("BitsPerPixel: %d\n", mode_info.bpp);
    serial_printf("PhysBasePtr: %x\n", mode_info.phys_base);
    serial_printf("Pitch: %d\n", mode_info.pitch);

/*
    for(uint32_t y = 0; y < vesa_getYResolution(); y++)
    {
        for(uint32_t x = 0; x < vesa_getXResolution(); x++)
        {
            if(y >= 600)
                vesa_putPixel(x, y, rgb(0, 0, 0));
            else if(y >= 400)
                vesa_putPixel(x, y, rgb(0, 0, 255));
            else if(y >= 200)
                vesa_putPixel(x, y, rgb(0, 255, 0));
            else
                vesa_putPixel(x, y, rgb(255, 0, 0));
        }
    }
    */
}

bool isVesaInit()
{
    return isVesaInitialized == 1;
}