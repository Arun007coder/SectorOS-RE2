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

#include "shell.h"
#include "keyboard.h"
#include "apm.h"

char shell_buffer[SHELL_BUFFER_SIZE];
uint8_t shell_buffer_index = 0;

char* curr_dir;

void shell_interpreter()
{
    printf("\n");
    if (strcmp(shell_buffer, "help") == 0)
    {
        printf("help:\n");
        printf("\t- Prints this help message\n");
        printf("clear:\n");
        printf("\t- Clears the screen\n");
        printf("echo [String]:\n");
        printf("\t- Prints the given string\n");
        printf("sysfetch:\n");
        printf("\t- gets information about the system\n");
        printf("cd [Directory]:\n");
        printf("\t- Changes the current directory\n");
        printf("ls [Directory]:\n");
        printf("\t- Lists the contents of the current directory\n");
        printf("cat [Filename]:\n");
        printf("\t- Prints the contents of the given file\n");
        printf("write [Filename] [Content]:\n");
        printf("\t- Writes the given content to the given file\n");
        printf("mount [Device filename] [mountpoint]:\n");
        printf("\t- Mounts the given device to the given mountpoint\n");
        printf("unmount [mountpoint]:\n");
        printf("\t- Unmounts the given mountpoint\n");
        printf("touch [Filename]:\n");
        printf("\t- Creates a new file\n");
        printf("time:\n");
        printf("\t- Prints the ticks since boot\n");
        printf("license:\n");
        printf("\t- Prints the license\n");
        printf("exit:\n");
        printf("\t- resets the system\n");
        printf("crdsk [Filename]:\n");
        printf("\t- Creates a ramdisk\n");
        printf("del [Filename]:\n");
        printf("\t- Deletes a file\n");
        printf("uname [-anbvh]:\n");
        printf("\t-Prints about the kernel\n");
        printf("rand:\n");
        printf("\t-Prints a random number\n");
    }
    else if(strcmp(shell_buffer, "shutdown") == 0 || strcmp(shell_buffer, "sd") == 0)
    {
        printf("Shutting down...\n");
        apm_shutdown();
    }
    else if(strcmp(shell_buffer, "reboot") == 0 || strcmp(shell_buffer, "rb") == 0)
    {
        printf("Rebooting...\n");
        reboot();
    }
    else if (strcmp(shell_buffer, "clear") == 0)
    {
        clear();
    }
    else if (strstr(shell_buffer, "echo") != NULL)
    {
        for (uint8_t i = 5; i < shell_buffer_index; i++)
        {
            printf("%c", shell_buffer[i]);
        }
        printf("\n");
    }
    else if (strcmp(shell_buffer, "exit") == 0)
    {
        printf("Exiting shell...\n");
        reboot();
        return;
    }
    else if(strstr(shell_buffer, "ls") != NULL)
    {
        char* filename = strstr(shell_buffer, "ls") + 3;
        filename[strlen(filename)] = '\0';
        if(filename[0] == '\0')
        {
            VFS_db_listdir(curr_dir);
            serial_printf("curr_dir: %s\n", curr_dir);
        }
        else
            VFS_db_listdir(filename);
    }
    else if(strstr(shell_buffer, "cd") != NULL)
    {
        char* filename = strstr(shell_buffer, "cd") + 3;
        filename[strlen(filename)] = '\0';
        memset(curr_dir, 0, 512);
        if(filename[0] == '\0')
        {
            strcpy(curr_dir, "/");
        }
        else
        {
            strcpy(curr_dir, filename);
        }
    }
    else if(shell_buffer[0] == 'm' && shell_buffer[1] == 'o' && shell_buffer[2] == 'u' && shell_buffer[3] == 'n' && shell_buffer[4] == 't')
    {
        uint32_t n;
        char* arg = strdup(shell_buffer + 6);
        list_t* args = str_split(arg, " ", &n);

        char* mountpoint = list_pop(args)->val;
        char* devpath = list_pop(args)->val;

        mount(devpath, mountpoint);
    }
    else if(shell_buffer[0] == 'l' && shell_buffer[1] == 'i' && shell_buffer[2] == 'c' && shell_buffer[3] == 'e' && shell_buffer[4] == 'n' && shell_buffer[5] == 'c' && shell_buffer[6] == 'e')
    {
        printf("Licence: GPLv3\n");
        printf("Copyright (C) 2022 Arun007coder\n");
    }
    else if(shell_buffer[0] == 'c' && shell_buffer[1] == 'r' && shell_buffer[2] == 'd' && shell_buffer[3] == 's' && shell_buffer[4] == 'k')
    {
        char* filename = kmalloc(strlen(shell_buffer) - 6 );

        for(uint8_t i = 6; i < strlen(shell_buffer); i++)
        {
            filename[i - 6] = shell_buffer[i];
        }

        filename[strlen(shell_buffer) - 6] = '\0';

        printf("Creating ramdisk from: %s\n", filename);

        ramdisk_createFromFile(filename);
    }
    else if(shell_buffer[0] == 'd' && shell_buffer[1] == 'e' && shell_buffer[2] == 'l')
    {
        char* filename = kmalloc(strlen(shell_buffer) - 4 );

        for(uint8_t i = 4; i < strlen(shell_buffer); i++)
        {
            filename[i - 4] = shell_buffer[i];
        }

        filename[strlen(shell_buffer) - 4] = '\0';

        VFS_unlink(filename);
    }
    else if(shell_buffer[0] == 'u' && shell_buffer[1] == 'n' && shell_buffer[2] == 'm' && shell_buffer[3] == 'o' && shell_buffer[4] == 'u' && shell_buffer[5] == 'n')
    {
        uint32_t n;
        char* arg = strdup(shell_buffer + 7);
        list_t* args = str_split(arg, " ", &n);

        char* mountpoint = list_pop(args)->val;

        VFS_unmount(mountpoint);
    }
    else if(strstr(shell_buffer, "cat") != NULL)
    {
        char* filename = strstr(shell_buffer, "cat") + 4;
        filename[strlen(filename)] = '\0';
        vfs_node* node;
        if(filename[0] != '/')
        {
            char* path = (char*)kmalloc(strlen(curr_dir) + strlen(filename) + 1);
            strcpy(path, curr_dir);
            strcat(path, "/");
            strcat(path, filename);
            node = file_open(path, 0);
            kfree(path);
        }
        else
        {
            node = file_open(filename, 0);
        }
        
        if(node != NULL)
        {
            uint32_t size = VFS_getFileSize(node);
            char* buffer = (char*)kmalloc((sizeof(char) * (size ? size : 1024)));
            memset(buffer, 0, size);
            VFS_read(node, 0, size, buffer);
            printf("\n%s", buffer);
            kfree(buffer);
        }
        else
        {
            printf("File %s could not be opened\n", filename);
        }
    }
    else if(shell_buffer[0] == 'w' && shell_buffer[1] == 'r' && shell_buffer[2] == 'i' && shell_buffer[3] == 't' && shell_buffer[4] == 'e')
    {
        char* filename = kmalloc(512);
        char* buffer = kmalloc(512);
        uint32_t i = 0;
        for(i = 6; shell_buffer[i] != ' '; i++)
        {
            filename[i - 6] = shell_buffer[i];
        }
        filename[i - 6] = '\0';
        i++;
        
        uint32_t j = 0;
        for(; i < shell_buffer_index; i++)
        {
            buffer[j] = shell_buffer[i];
            j++;
        }
        buffer[j] = '\0';
        vfs_node* node;
        if(filename[0] != '/')
        {
            char* path = (char*)kmalloc(strlen(curr_dir) + strlen(filename) + 1);
            strcpy(path, curr_dir);
            strcat(path, "/");
            strcat(path, filename);
            node = file_open(path, OPEN_WRONLY);
            kfree(path);
        }
        else
        {
            node = file_open(filename, OPEN_WRONLY);
        }
        
        if(node != NULL)
        {
            VFS_write(node, 0, strlen(buffer), buffer);
            //printf("File written successfully\n");
        }
        else
        {
            printf("File %s could not be opened\n", filename);
        }
    }
    else if (strncmp(shell_buffer, "touch", 5) == 0)
    {
        char* l = kmalloc(strlen(shell_buffer) - 6);
        strcpy(l, shell_buffer + 6);
        l[strlen(l)] = '\0';
        VFS_create(l, 0);
        printf("File %s created successfully\n", l);
        kfree(l);
    }
    else if(shell_buffer[0] == 'u' && shell_buffer[1] == 'n' && shell_buffer[2] == 'a' && shell_buffer[3] == 'm' && shell_buffer[4] == 'e')
    {
        char* arg = strdup(shell_buffer + 6);
        if(arg[0] != '-')
        {
            printf("%s version %s %s x86\n", KERNEL_NAME, KERNEL_VERSION, KERNEL_BUILD);
        }
        else
        {
            if(arg[1] == 'a')
            {
                printf("%s version %s %s x86\n", KERNEL_NAME, KERNEL_VERSION, KERNEL_BUILD);
            }
            else if(arg[1] == 'n')
            {
                printf("%s\n", KERNEL_NAME);
            }
            else if(arg[1] == 'b')
            {
                printf("%s\n", KERNEL_BUILD);
            }
            else if(arg[1] == 'v')
            {
                printf("%s\n", KERNEL_VERSION);
            }
            else if(arg[1] == 'h')
            {
                printf("Usage: uname [-anbvh]\n");
            }
        }
    }
    else if (strcmp(shell_buffer, "rand") == 0)
    {
        printf("%d\n", abs(rand()));
    }
    else if (strcmp(shell_buffer, "sysfetch") == 0)
    {
        printf("OS: %s\n", KERNEL_NAME);
        printf("Kernel: %s\n", KERNEL_VERSION);
        printf("Kernel base: 0x%06x\n", KERNEL_BASE);
        printf("Kernel end: 0x%06x\n", (uint32_t)&end);
        printf("memory allocator is %s\n", (kheap_enabled) ? "disabled" : "enabled");
        printf("pageing is %s\n", (paging_enabled) ? "enabled" : "disabled");
        printf("builded on %s using %s\n", KERNEL_BUILD, COMPILER);
        printf("Resolution: %dx%d\n", VGA_WIDTH, VGA_HEIGHT);
        printf("Shell: %s\n", SOSH_VERSION);
        print_cpuinfo();
        multiboot_info_t* mboot = (multiboot_info_t*)mboot_addr;
        printf("Memory: %s\n", stoc_r((mboot->mem_upper * 1024) + (mboot->mem_lower * 1024)));

        printf("\n");
        for (uint8_t i = 0; i < 32; i++)
        {
            if(i % 16 == 0)
            {
                change_color(VGA_LIGHT_GREEN, VGA_BLACK);
                printf("\n");
            }
            change_color(0, i);
            printf(" ");
        }
        change_color(VGA_LIGHT_GREEN, VGA_BLACK);
        printf("\n");
    }
    else if (strcmp(shell_buffer, "time") == 0)
    {
        printf("%d\n", time_since_boot);
    }
    else
    {
        printf("Unknown command: %s\n", shell_buffer);
    }
    printf("%s#> ", curr_dir);
    serial_printf("current directory: %s\n", curr_dir);
    memset(shell_buffer, 0, SHELL_BUFFER_SIZE);
    shell_buffer_index = 0;
}

bool isShifted;
bool isCTRLed;

char KCTC(uint8_t keyCode)
{
    char result = 0;
    if (keyCode < 0x80)
    {
        switch (keyCode)
        {
        case 0xFA:
            break;
        case 0x45:
        case 0xC5:
            break;
        case 0x0E:
            result = '\b';
            break;
        case 0x0F:
            result = '\t';
            break;
        case 0x29:
            if (!isShifted)
                result = '`';
            else
                result = '~';
            break;
        case 0x02:
            if (!isShifted)
                result = '1';
            else
                result = '!';
            break;
        case 0x03:
            if (!isShifted)
                result = '2';
            else
                result = '@';
            break;
        case 0x04:
            if (!isShifted)
                result = '3';
            else
                result = '#';
            break;
        case 0x05:
            if (!isShifted)
                result = '4';
            else
                result = '$';
            break;
        case 0x06:
            if (!isShifted)
                result = '5';
            else
                result = '%';
            break;
        case 0x07:
            if (!isShifted)
                result = '6';
            else
                result = '^';
            break;
        case 0x08:
            if (!isShifted)
                result = '7';
            else
                result = '&';
            break;
        case 0x09:
            if (!isShifted)
                result = '8';
            else
                result = '*';
            break;
        case 0x0A:
            if (!isShifted)
                result = '9';
            else
                result = '(';
            break;
        case 0x0B:
            if (!isShifted)
                result = '0';
            else
                result = ')';
            break;
        case 0x0C:
            if (!isShifted)
                result = '-';
            else
                result = '_';
            break;
        case 0x0D:
            if (!isShifted)
                result = '=';
            else
                result = '+';
            break;

        case 0x10:
            if (!isShifted)
                result = 'q';
            else
                result = 'Q';
            break;
        case 0x11:
            if (!isShifted)
                result = 'w';
            else
                result = 'W';
            break;
        case 0x12:
            if (!isShifted)
                result = 'e';
            else
                result = 'E';
            break;
        case 0x13:
            if (!isShifted)
                result = 'r';
            else
                result = 'R';
            break;
        case 0x14:
            if (!isShifted)
                result = 't';
            else
                result = 'T';
            break;
        case 0x15:
            if (!isShifted)
                result = 'y';
            else
                result = 'Y';
            break;
        case 0x16:
            if (!isShifted)
                result = 'u';
            else
                result = 'U';
            break;
        case 0x17:
            if (!isShifted)
                result = 'i';
            else
                result = 'I';
            break;
        case 0x18:
            if (!isShifted)
                result = 'o';
            else
                result = 'O';
            break;
        case 0x19:
            if (!isShifted)
                result = 'p';
            else
                result = 'P';
            break;
        case 0x1A:
            if (!isShifted)
                result = '[';
            else
                result = '{';
            break;
        case 0x1B:
            if (!isShifted)
                result = ']';
            else
                result = '}';
            break;
        case 0x1C:
            if (!isShifted)
                result = '\n';
            else
                result = '\n';
            break;

        case 0x1E:
            if (!isShifted)
                result = 'a';
            else
                result = 'A';
            break;
        case 0x1F:
            if (!isShifted)
                result = 's';
            else
                result = 'S';
            break;
        case 0x20:
            if (!isShifted)
                result = 'd';
            else
                result = 'D';
            break;
        case 0x21:
            if (!isShifted)
                result = 'f';
            else
                result = 'F';
            break;
        case 0x22:
            if (!isShifted)
                result = 'g';
            else
                result = 'G';
            break;
        case 0x23:
            if (!isShifted)
                result = 'h';
            else
                result = 'H';
            break;
        case 0x24:
            if (!isShifted)
                result = 'j';
            else
                result = 'J';
            break;
        case 0x25:
            if (!isShifted)
                result = 'k';
            else
                result = 'K';
            break;
        case 0x26:
            if (!isShifted)
                result = 'l';
            else
                result = 'L';
            break;
        case 0x27:
            if (!isShifted)
                result = ';';
            else
                result = ':';
            break;

        case 0x2C:
            if (!isShifted)
                result = 'z';
            else
                result = 'Z';
            break;
        case 0x2D:
            if (!isShifted)
                result = 'x';
            else
                result = 'X';
            break;
        case 0x2E:
            if (!isShifted)
                result = 'c';
            else
                result = 'C';
            break;
        case 0x2F:
            if (!isShifted)
                result = 'v';
            else
                result = 'V';
            break;
        case 0x30:
            if (!isShifted)
                result = 'b';
            else
                result = 'B';
            break;
        case 0x31:
            if (!isShifted)
                result = 'n';
            else
                result = 'N';
            break;
        case 0x32:
            if (!isShifted)
                result = 'm';
            else
                result = 'M';
            break;
        case 0x33:
            if (!isShifted)
                result = ',';
            else
                result = '<';
            break;
        case 0x34:
            if (!isShifted)
                result = '.';
            else
                result = '>';
            break;
        case 0x35:
            if (!isShifted)
                result = '/';
            else
                result = '?';
            break;
        case 0x39:
            if (!isShifted)
                result = ' ';
            else
                result = ' ';
            break;
        case 0x2B:
            if (!isShifted)
                result = '\\';
            else
                result = '|';
            break;
        }
    }
    return result;
}

void init_shell()
{
    curr_dir = kmalloc(512);
    strcpy(curr_dir, "/");
    init_keyboard(shell_run);
}

void shell_run(uint8_t cmd)
{
    if(cmd < 80)
    {
        if(KCTC(cmd) != '\n' & cmd != 0x45 & cmd != 0x61)
        {
            shell_buffer[shell_buffer_index] = KCTC(cmd);
            if(KCTC(cmd) == '\b')
            {
                shell_buffer_index--;
            }
            else
            {
                shell_buffer_index++;
            }
        }
    }

    switch (cmd)
    {
        case 0xFA: break;
        case 0x45: case 0xC5: case 0x61: break;
        case 0x29:if (!isShifted) printf("`"); else printf("~"); break;
        case 0x02:if (!isShifted) printf("1"); else printf("!"); break;
        case 0x03:if (!isShifted) printf("2"); else printf("@"); break;
        case 0x04:if (!isShifted) printf("3"); else printf("#"); break;
        case 0x05:if (!isShifted) printf("4"); else printf("$"); break;
        case 0x06:if (!isShifted) printf("5"); else printf("%%"); break;
        case 0x07:if (!isShifted) printf("6"); else printf("^"); break;
        case 0x08:if (!isShifted) printf("7"); else printf("&"); break;
        case 0x09:if (!isShifted) printf("8"); else printf("*"); break;
        case 0x0A:if (!isShifted) printf("9"); else printf("("); break;
        case 0x0B:if (!isShifted) printf("0"); else printf(")"); break;
        case 0x0C:if (!isShifted) printf("-"); else printf("_"); break;
        case 0x0D:if (!isShifted) printf("="); else printf("+"); break;
        case 0x0F: printf("\t"); break;
        case 0x10:if (!isShifted) printf("q"); else printf("Q"); break;
        case 0x11:if (!isShifted) printf("w"); else printf("W"); break;
        case 0x12:if (!isShifted) printf("e"); else printf("E"); break;
        case 0x13:if (!isShifted) printf("r"); else printf("R"); break;
        case 0x14:if (!isShifted) printf("t"); else printf("T"); break;
        case 0x15:if (!isShifted) printf("y"); else printf("Y"); break;
        case 0x16:if (!isShifted) printf("u"); else printf("U"); break;
        case 0x17:if (!isShifted) printf("i"); else printf("I"); break;
        case 0x18:if (!isShifted) printf("o"); else printf("O"); break;
        case 0x19:if (!isShifted) printf("p"); else printf("P"); break;
        case 0x1A:if (!isShifted) printf("["); else printf("{"); break;
        case 0x1B:if (!isShifted) printf("]"); else printf("}"); break;
        case 0x1C: 
            shell_buffer[shell_buffer_index] = '\0';
            shell_interpreter();
        break; //Enter
        case 0x1E:if (!isShifted) printf("a"); else printf("A"); break;
        case 0x1F:if (!isShifted) printf("s"); else printf("S"); break;
        case 0x20:if (!isShifted) printf("d"); else printf("D"); break;
        case 0x21:if (!isShifted) printf("f"); else printf("F"); break;
        case 0x22:if (!isShifted) printf("g"); else printf("G"); break;
        case 0x23:if (!isShifted) printf("h"); else printf("H"); break;
        case 0x24:if (!isShifted) printf("j"); else printf("J"); break;
        case 0x25:if (!isShifted) printf("k"); else printf("K"); break;
        case 0x26:if (!isShifted) printf("l"); else printf("L"); break;
        case 0x27:if (!isShifted) printf(";"); else printf(":"); break;
        case 0x28:if (!isShifted) printf("E"); else printf("e"); break;
        case 0x2B:if (!isShifted) printf("\\"); else printf("|"); break;
        case 0x2A: isShifted = true; break;
        case 0x2C:if (!isShifted) printf("z"); else printf("Z"); break;
        case 0x2D:if (!isShifted) printf("x"); else printf("X"); break;
        case 0x2E: if (!isShifted) printf("c"); else printf("C");break;
        case 0x2F:if (!isShifted) printf("v"); else printf("V"); break;
        case 0x30:if (!isShifted) printf("b"); else printf("B"); break;
        case 0x31:if (!isShifted) printf("n"); else printf("N"); break;
        case 0x32:if (!isShifted) printf("m"); else printf("M"); break;
        case 0x33:if (!isShifted) printf(","); else printf("<"); break;
        case 0x34:if (!isShifted) printf("."); else printf(">"); break;
        case 0x35:if (!isShifted) printf("/"); else printf("?"); break;
        case 0x39: printf(" "); break;
        case 0x36: isShifted = true; break;
        case 0xAA: case 0xB6: isShifted = false; break;
        case 0x3A: if(isShifted){
            isShifted = false;
        }
        else{
            isShifted = true;
        }
        break;
        case 0x1D:
            isCTRLed = true;
        break;
        case 0xE0:
            isCTRLed = false;
        break;
        case 0x9D:
            isCTRLed = false;
        break;
        case 0x0E:
            printf("\b");
            break;
        default:
            if (cmd < 0x80){
                printf(" KEYBOARD 0x%02X ", cmd);
            }
        break;
    }
    
}