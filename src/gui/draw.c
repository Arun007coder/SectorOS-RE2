#include "draw.h"

uint32_t * screen = NULL;
uint32_t fill_color;
int screen_width, screen_height;

int get_pixel_index(canvas_t * canvas, int x, int y)
{
    return canvas->width * y + x;
}

void set_pixel(canvas_t * canvas, uint32_t val, int x, int y)
{
    canvas->framebuffer[get_pixel_index(canvas, x, y)] = val;
}

void set_fill_color(uint32_t color)
{
    fill_color = color;
}

void remove_sharp_edges(canvas_t * canvas, int start_x, int start_y, int direction, int num_pixels, uint32_t end_alpha, uint32_t middle_alpha)
{
    uint32_t idx, color, count, alpha;
    int i, j;
    i = start_y; j = start_x, count = num_pixels;

    while(count-- > 0)
    {
        idx = get_pixel_index(canvas, j, i);
        color = canvas->framebuffer[idx];
        if(count == 0 || count == num_pixels - 1)
            alpha = end_alpha;
        else
            alpha = middle_alpha;
        set_pixel(canvas, SET_ALPHA(color, alpha), j, i);
        i++;
        j += direction;
    }
}

void round_corner_effect(canvas_t * canvas)
{
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3 - i; j++)
        {
            set_pixel(canvas, 0x0, j, i);
            set_pixel(canvas, 0x0, canvas->width - 3 + i + j, i);
        }
    }

    remove_sharp_edges(canvas, 0, canvas->width - 4, 1, 4, 0x88, 0xee);
    remove_sharp_edges(canvas, 0, canvas->width - 5, 1, 5, 0xff, 0xee);
    remove_sharp_edges(canvas, 1, canvas->width - 5, 1, 4, 0xff, 0xff);

    remove_sharp_edges(canvas, 0, 3, -1, 4, 0x88, 0xee);
    remove_sharp_edges(canvas, 0, 4, -1, 5, 0x88, 0xee);
    remove_sharp_edges(canvas, 1, 4, -1, 4, 0x88, 0xee);
}

void draw_rect(canvas_t * canvas, int x, int y, int width, int height)
{
    for(int i = y; i < y + height; i++)
    {
        for(int j = x; j < x + width; j++)
        {
            set_pixel(canvas, fill_color, j, i);
        }
    }
}

void draw_rect_pixels(canvas_t * canvas, rect_region_t * rect_region)
{
    for(int i = rect_region->r.y;  i < rect_region->r.y + rect_region->r.height; i++)
    {
        for(int j = rect_region->r.x; j < rect_region->r.x + rect_region->r.width; j++)
        {
            int idx = rect_region->r.width * (i - rect_region->r.y) + (j - rect_region->r.x);
            if(rect_region->region[idx] != 0x0)
                set_pixel(canvas, rect_region->region[idx], j, i);
        }
    }
}

void draw_rect_clip_pixels(canvas_t * canvas, rect_region_t * rect_region, int rect_true_width)
{
    for(int i = rect_region->r.y;  i < rect_region->r.y + rect_region->r.height; i++)
    {
        for(int j = rect_region->r.x; j < rect_region->r.x + rect_region->r.width; j++)
        {
            int idx = rect_true_width * (i - rect_region->r.y) + (j - rect_region->r.x);
            if(rect_region->region[idx] != 0x0)
                set_pixel(canvas, rect_region->region[idx], j, i);
        }
    }
}

void draw_rect_clip_pixels2(canvas_t * canvas, rect_region_t * rect_region, int rect_true_width, int rect_true_x, int rect_true_y)
{
    for(int i = rect_region->r.y;  i < rect_region->r.y + rect_region->r.height; i++)
    {
        for(int j = rect_region->r.x; j < rect_region->r.x + rect_region->r.width; j++)
        {
            int idx = rect_true_width * (i - rect_true_y) + (j - rect_true_x);
            if(rect_region->region[idx] != 0x0)
                set_pixel(canvas, rect_region->region[idx], j, i);
        }
    }
}

void draw_line(canvas_t * canvas, int x1, int y1, int x2, int y2)
{
    int dx = x2-x1;
    int dy = y2-y1;
    int dxabs = abs(dx);
    int dyabs = abs(dy);
    int sdx = sign(dx);
    int sdy = sign(dy);
    int x = 0;
    int y = 0;
    int px = x1;
    int py = y1;

    set_pixel(canvas, fill_color, px, py);
    if (dxabs>=dyabs)
    {
        for(int i=0;i<dxabs;i++)
        {
            y+=dyabs;
            if (y>=dxabs)
            {
                y-=dxabs;
                py+=sdy;
            }
            px+=sdx;
            set_pixel(canvas, fill_color, px, py);
        }
    }
    else
    {
        for(int i=0;i<dyabs;i++)
        {
            x+=dxabs;
            if (x>=dyabs)
            {
                x-=dyabs;
                px+=sdx;
            }
            py+=sdy;
            set_pixel(canvas, fill_color, px, py);
        }
    }
}

int is_line_overlap(int line1_x1, int line1_x2, int line2_x1, int line2_x2)
{
    int ret1 = line1_x1 < line2_x1 && (line1_x1 >= line2_x1 && line1_x2 <= line2_x2);
    int ret2 = (line1_x1 >= line2_x1 && line1_x1 <= line2_x2) && (line1_x2 >= line2_x1 && line1_x2 <= line2_x2);
    int ret3 = (line1_x1 >= line2_x1 && line1_x1 <= line2_x2) && (line1_x2 > line2_x2);
    int ret4 = (line1_x1 < line2_x1) && (line1_x1 > line2_x2);
    return ret1 || ret2 || ret3 || ret4;
}

int is_rect_overlap(rect_t rect1, rect_t rect2)
{
    if((rect1.x > rect2.x + rect2.width) || (rect1.x + rect1.width < rect2.x))
        return 0;
    if((rect1.y > rect2.y + rect2.height) || (rect1.y + rect1.height < rect2.y))
        return 0;
    return 1;

}

rect_t find_rect_overlap(rect_t rect1, rect_t rect2)
{
    rect_t ret;
    if(rect1.x + rect1.width > rect2.x + rect2.width)
    {
        ret.width = rect2.x + rect2.width - max(rect1.x, rect2.x);
    }
    else
    {
        ret.width = rect1.x + rect1.width - max(rect1.x, rect2.x);
    }
    ret.x = max(rect1.x, rect2.x);
    if(rect1.y + rect1.height > rect2.y + rect2.height)
    {
        ret.height = rect2.y + rect2.height - max(rect1.y, rect2.y);
    }
    else
    {
        ret.height = rect1.y + rect1.height - max(rect1.y, rect2.y);
    }
    ret.y = max(rect1.y, rect2.y);
    return ret;
}


int is_point_in_rect(int point_x, int point_y, rect_t * r)
{
    return (point_x >= r->x && point_x < r->x + r->width) && (point_y >= r->y && point_y < r->y + r->height);
}

rect_t rect_create(int x, int y, int width, int height)
{
    rect_t r;
    r.x = x;
    r.y = y;
    r.width = width;
    r.height = height;
    return r;
}

canvas_t canvas_create(int width, int height, uint32_t * framebuffer)
{
    canvas_t canvas;
    canvas.width = width;
    canvas.height = height;
    canvas.framebuffer = framebuffer;
    return canvas;
}