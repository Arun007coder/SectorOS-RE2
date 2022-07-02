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

#include "system.h"
#include "string.h"
#include "printf.h"

void intel()
{
    printf("CPU manufacturer: GenuineIntel\n");
    uint32_t eax, ebx, ecx, edx;
    CPUID(1, eax, ebx, ecx, edx);
    uint32_t brand = eax & 0xf;
    uint32_t model = (eax >> 4) & 0xf;
    uint32_t family = (eax >> 8) & 0xf;
    uint32_t type = (eax >> 12) & 0xf;
    switch(type)
    {
    case 0:
        printf("CPU type: Original OEM Processor\n");
        break;
    case 1:
        printf("CPU type: Overdrive Processor\n");
        break;
    case 2:
        printf("CPU type: Dual-capable processor\n");
        break;
    case 3:
        printf("CPU type: Reserved\n");
        break;
    case 4:
        printf("CPU type: Quad-capable processor\n");
        break;
    }

    switch(family)
    {
    case 3:
        printf("CPU model: i386\n");
        break;
    case 4:
        switch(model)
        {
        case 0:
            break;
        case 1:
            printf("CPU model: i486 DX\n");
            break;
        case 2:
            printf("CPU model: i486 SX\n");
            break;
        case 3:
            printf("CPU model: i486 487/DX2\n");
            break;
        case 4:
            printf("CPU model: i486 SL\n");
            break;
        case 5:
            printf("CPU model: i486 SX2\n");
            break;
        case 7:
            printf("CPU model: i486 write-back enhanced DX2\n");
            break;
        case 8:
            printf("CPU model: i486 DX4\n");
            break;
        default:
            printf("CPU model: i486\n");
            break;
        }
        break;
    case 5:
        switch(model)
        {
        case 1:
            printf("CPU model: Pentium 60/66 A-step\n");
            break;
        case 2:
            printf("CPU model: Pentium 75 - 200 A-step\n");
            break;
        case 3:
            printf("CPU model: Pentium for 486 system\n");
            break;
        case 4:
            printf("CPU model: Pentium MMX\n");
            break;
        default:
            printf("CPU model: Pentium\n");
            break;
        }
        break;
    case 6:
        switch(model)
        {
        case 1:
            printf("CPU model: Pentium Pro A-step\n");
            break;
        case 3:
            printf("CPU model: Pentium II Model 3\n");
            break;
        case 5:
            printf("CPU model: Pentium II Model 5/Xeon/Celeron\n");
            break;
        case 6:
            printf("CPU model: Pentium II/Celeron Model 6\n");
            break;
        case 7:
            printf("CPU model: Pentium III/Pentium III Xeon - External L2 Cache\n");
            break;
        case 8:
            printf("CPU model: Pentium III/Pentium III Xeon - Internal L2 Cache\n");
            break;
        default:
            printf("CPU model: Pentium Pro\n");
            break;
        }
        break;
    case 15:
        switch(model)
        {
        default:
            printf("CPU model: Pentium 4\n");
            break;
        }
        break;
    }

}

void amd()
{
    printf("CPU manufacturer: AuthenticAMD\n");
}

void print_cpuinfo()
{
    uint32_t ebx, unused;
    CPUID(0, unused, ebx, unused, unused);
    switch(ebx)
    {
    case 0x756e6547:
        intel();
        break;
    case 0x68747541:
        amd();
        break;
    case 0x69727943:
        printf("CPU: Cyrix\n");
        break;
    case 0x746e6543:
        printf("CPU: Centaur\n");
        break;
    case 0x746e6542:
        printf("CPU: NSC\n");
        break;
    default:
        printf("CPU: Unknown\n");
        break;
    }
}

