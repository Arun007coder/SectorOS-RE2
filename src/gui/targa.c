/**
 * Copyright (C) 2022 Arun007coder
 * 
 * This file is part of SectorOS-RE2.
 * 
 * SectorOS-RE2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * SectorOS-RE2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with SectorOS-RE2.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "targa.h"

uint32_t* targa_parse(uint8_t* buffer, uint32_t size)
{
    uint32_t *data;
    int i, j, k, x, y, w = (buffer[13] << 8) + buffer[12], h = (buffer[15] << 8) + buffer[14], o = (buffer[11] << 8) + buffer[10];
    int m = ((buffer[1]? (buffer[7]>>3)*buffer[5] : 0) + 18);
    if(w<1 || h<1) return NULL;
    data = (uint32_t*)kmalloc((w*h+2)*sizeof(uint32_t));
    if(!data) return NULL;

    switch(buffer[2]) {
        case 1:
            if(buffer[6]!=0 || buffer[4]!=0 || buffer[3]!=0 || (buffer[7]!=24 && buffer[7]!=32)) { free(data); return NULL; }
            for(y=i=0; y<h; y++) {
                k = ((!o?h-y-1:y)*w);
                for(x=0; x<w; x++) {
                    j = buffer[m + k++]*(buffer[7]>>3) + 18;
                    data[2 + i++] = ((buffer[7]==32?buffer[j+3]:0xFF) << 24) | (buffer[j+2] << 16) | (buffer[j+1] << 8) | buffer[j];
                }
            }
            break;
        case 2:
            if(buffer[5]!=0 || buffer[6]!=0 || buffer[1]!=0 || (buffer[16]!=24 && buffer[16]!=32)) { free(data); return NULL; }
            for(y=i=0; y<h; y++) {
                j = ((!o?h-y-1:y)*w*(buffer[16]>>3));
                for(x=0; x<w; x++) {
                    data[2 + i++] = ((buffer[16]==32?buffer[j+3]:0xFF) << 24) | (buffer[j+2] << 16) | (buffer[j+1] << 8) | buffer[j];
                    j += buffer[16]>>3;
                }
            }
            break;
        case 9:
            if(buffer[6]!=0 || buffer[4]!=0 || buffer[3]!=0 || (buffer[7]!=24 && buffer[7]!=32)) { free(data); return NULL; }
            y = i = 0;
            for(x=0; x<w*h && m<size;) {
                k = buffer[m++];
                if(k > 127) {
                    k -= 127; x += k;
                    j = buffer[m++]*(buffer[7]>>3) + 18;
                    while(k--) {
                        if(!(i%w)) { i=((!o?h-y-1:y)*w); y++; }
                        data[2 + i++] = ((buffer[7]==32?buffer[j+3]:0xFF) << 24) | (buffer[j+2] << 16) | (buffer[j+1] << 8) | buffer[j];
                    }
                } else {
                    k++; x += k;
                    while(k--) {
                        j = buffer[m++]*(buffer[7]>>3) + 18;
                        if(!(i%w)) { i=((!o?h-y-1:y)*w); y++; }
                        data[2 + i++] = ((buffer[7]==32?buffer[j+3]:0xFF) << 24) | (buffer[j+2] << 16) | (buffer[j+1] << 8) | buffer[j];
                    }
                }
            }
            break;
        case 10:
            if(buffer[5]!=0 || buffer[6]!=0 || buffer[1]!=0 || (buffer[16]!=24 && buffer[16]!=32)) { free(data); return NULL; }
            y = i = 0;
            for(x=0; x<w*h && m<size;) {
                k = buffer[m++];
                if(k > 127) {
                    k -= 127; x += k;
                    while(k--) {
                        if(!(i%w)) { i=((!o?h-y-1:y)*w); y++; }
                        data[2 + i++] = ((buffer[16]==32?buffer[m+3]:0xFF) << 24) | (buffer[m+2] << 16) | (buffer[m+1] << 8) | buffer[m];
                    }
                    m += buffer[16]>>3;
                } else {
                    k++; x += k;
                    while(k--) {
                        if(!(i%w)) { i=((!o?h-y-1:y)*w); y++; }
                        data[2 + i++] = ((buffer[16]==32?buffer[m+3]:0xFF) << 24) | (buffer[m+2] << 16) | (buffer[m+1] << 8) | buffer[m];
                        m += buffer[16]>>3;
                    }
                }
            }
            break;
        default:
            kfree(data); return NULL;
    }
    data[0] = w;
    data[1] = h;
    return data;
}

void targa_display(char* name)
{
    uint32_t *data;
    int i, j, k, x, y, w, h;
    uint8_t *buffer;
    FILE *f = file_open(name, 0);
    if(!f) return;

    buffer = (uint8_t*)kmalloc(f->size);
    if(!buffer) return;

    VFS_read(f, 0, f->size, buffer);

    data = targa_parse(buffer, f->size);

    if(!data) {
        kfree(buffer);
        return;
    }

    w = data[0];
    h = data[1];
    kfree(buffer);

    uint32_t* framebuffer = (uint32_t*)vesa_getFramebuffer();

    for(y=0; y<h; y++) {
        for(x=0; x<w; x++) {
            framebuffer[y*vesa_getXResolution() + x] = data[2 + y*w + x];
        }
    }

    kfree(data);
}