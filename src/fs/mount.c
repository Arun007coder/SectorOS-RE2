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

#include "mount.h"

#include "sorfs.h"
#include "ext2.h"

#include "vfs.h"

int mount(char* devpath, char* mountpoint)
{
    uint32_t t = find_fs(devpath);

    if(t == FS_TYPE_EXT2)
    {
        EXT2_init(devpath, mountpoint);
        return 0;
    }
    else if(t == FS_TYPE_SORFS)
    {
        init_sorfs(devpath, mountpoint);
        return 0;
    }
    else
    {
        return -1;
    }
}