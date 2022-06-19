#include "bitmap.h"

BITMAP_t * bitmap_create(char * filename)
{
    BITMAP_t * ret = kmalloc(sizeof(BITMAP_t));

    vfs_node * file = file_open(filename, 0);

    if(!file)
    {
        return NULL;
    }

    uint32_t size = VFS_getFileSize(file);
    void * buf = kmalloc(size);
    VFS_read(file, 0, size, buf);

    BITMAP_FILE_HEADER_t * h = buf;
    unsigned int offset = h->offset;

    BITMAP_INFO_HEADER_t * info = buf + sizeof(BITMAP_FILE_HEADER_t);

    ret->width = info->biWidth;
    ret->height = info->biHeight;
    ret->image_bytes= (void*)((unsigned int)buf + offset);
    ret->buffer = buf;
    ret->total_size= size;
    ret->bpp = info->biBitCount;
    VFS_close(file);
    return ret;
}

void bitmap_display(char* file)
{
    FILE* f = file_open(file, 0);
    if(!f)
    {

        serial_printf("Could not open file %s\n", file);
        return;
    }
    char * buf = kmalloc(VFS_getFileSize(f));
    VFS_read(f, 0, VFS_getFileSize(f), buf);

    BITMAP_t * b = bitmap_create(file);

    if(!b)
    {
        return;
    }

    if(b->height > vesa_getYResolution() || b->width > vesa_getXResolution())
    {
        serial_printf("Bitmap is too big!\n");
        return;
    }

    if(b->height < vesa_getYResolution() || b->width < vesa_getXResolution())
    {
        vesa_change_mode(b->width, b->height, b->bpp);
    }

    uint32_t* framebuffer = (uint32_t*)vesa_getFramebuffer();
    bitmap_to_framebuffer2(b, framebuffer);
}

void bitmap_to_framebuffer(BITMAP_t * bmp, uint32_t * frame_buffer)
{
    if(!bmp) return;
    uint8_t * image = bmp->image_bytes;
    int j = 0;
    for(int i = 0; i < bmp->height; i++)
    {
        char * image_row = image + i * bmp->width * 3;
        uint32_t * framebuffer_row = (void*)frame_buffer + (bmp->height - 1 - i) * bmp->width * 4;
        j = 0;
        for(int k = 0; k < bmp->width; k++) {
            uint32_t b = image_row[j++] & 0xff;
            uint32_t g = image_row[j++] & 0xff;
            uint32_t r = image_row[j++] & 0xff;
            uint32_t rgb = ((r << 16) | (g << 8) | (b)) & 0x00ffffff;
            rgb = rgb | 0xff000000;
            framebuffer_row[k] = rgb;
        }
    }
}
void bitmap_to_framebuffer2(BITMAP_t * bmp, uint32_t * frame_buffer) 
{
    if(!bmp) return;
    uint8_t * image = bmp->image_bytes;
    int j = 0;
    for(int i = 0; i < bmp->height; i++)
    {
        char * image_row = image + i * bmp->width * 4;
        uint32_t * framebuffer_row = (void*)frame_buffer + (bmp->height - 1 - i) * bmp->width * 4;
        j = 0;
        for(int k = 0; k < bmp->width; k++)
        {
            uint32_t b = image_row[j++] & 0xff;
            uint32_t g = image_row[j++] & 0xff;
            uint32_t r = image_row[j++] & 0xff;
            uint32_t a = image_row[j++] & 0xff;
            uint32_t rgba = ((a << 24) | (r << 16) | (g << 8) | (b));
            framebuffer_row[k] = rgba;
        }
    }
}