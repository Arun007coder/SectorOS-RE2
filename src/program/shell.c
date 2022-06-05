#include "shell.h"
#include "keyboard.h"

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
        return;
    }
    else if(strstr(shell_buffer, "fopen") != NULL)
    {
        char* filename = strstr(shell_buffer, "fopen") + 6;
        filename[strlen(filename)] = '\0';
        printf("Opening file: %s\n", filename);
        vfs_node* node = file_open(filename, 0);
        if(node != NULL)
        {
            printf("File opened successfully\n");
        }
        else
        {
            printf("File could not be opened\n");
        }
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
            printf("File could not be opened\n");
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
        printf("Shell: %s\n", "sosh 0.1.2");
        print_cpuinfo();
        multiboot_info_t* mboot = (multiboot_info_t*)mboot_addr;
        printf("Memory: ");
        stoc((mboot->mem_upper * 1024) + (mboot->mem_lower * 1024));
        printf("\n");

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

bool isShift;
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
            if (!isShift)
                result = '`';
            else
                result = '~';
            break;
        case 0x02:
            if (!isShift)
                result = '1';
            else
                result = '!';
            break;
        case 0x03:
            if (!isShift)
                result = '2';
            else
                result = '@';
            break;
        case 0x04:
            if (!isShift)
                result = '3';
            else
                result = '#';
            break;
        case 0x05:
            if (!isShift)
                result = '4';
            else
                result = '$';
            break;
        case 0x06:
            if (!isShift)
                result = '5';
            else
                result = '%';
            break;
        case 0x07:
            if (!isShift)
                result = '6';
            else
                result = '^';
            break;
        case 0x08:
            if (!isShift)
                result = '7';
            else
                result = '&';
            break;
        case 0x09:
            if (!isShift)
                result = '8';
            else
                result = '*';
            break;
        case 0x0A:
            if (!isShift)
                result = '9';
            else
                result = '(';
            break;
        case 0x0B:
            if (!isShift)
                result = '0';
            else
                result = ')';
            break;
        case 0x0C:
            if (!isShift)
                result = '-';
            else
                result = '_';
            break;
        case 0x0D:
            if (!isShift)
                result = '=';
            else
                result = '+';
            break;

        case 0x10:
            if (!isShift)
                result = 'q';
            else
                result = 'Q';
            break;
        case 0x11:
            if (!isShift)
                result = 'w';
            else
                result = 'W';
            break;
        case 0x12:
            if (!isShift)
                result = 'e';
            else
                result = 'E';
            break;
        case 0x13:
            if (!isShift)
                result = 'r';
            else
                result = 'R';
            break;
        case 0x14:
            if (!isShift)
                result = 't';
            else
                result = 'T';
            break;
        case 0x15:
            if (!isShift)
                result = 'y';
            else
                result = 'Y';
            break;
        case 0x16:
            if (!isShift)
                result = 'u';
            else
                result = 'U';
            break;
        case 0x17:
            if (!isShift)
                result = 'i';
            else
                result = 'I';
            break;
        case 0x18:
            if (!isShift)
                result = 'o';
            else
                result = 'O';
            break;
        case 0x19:
            if (!isShift)
                result = 'p';
            else
                result = 'P';
            break;
        case 0x1A:
            if (!isShift)
                result = '[';
            else
                result = '{';
            break;
        case 0x1B:
            if (!isShift)
                result = ']';
            else
                result = '}';
            break;
        case 0x1C:
            if (!isShift)
                result = '\n';
            else
                result = '\n';
            break;

        case 0x1E:
            if (!isShift)
                result = 'a';
            else
                result = 'A';
            break;
        case 0x1F:
            if (!isShift)
                result = 's';
            else
                result = 'S';
            break;
        case 0x20:
            if (!isShift)
                result = 'd';
            else
                result = 'D';
            break;
        case 0x21:
            if (!isShift)
                result = 'f';
            else
                result = 'F';
            break;
        case 0x22:
            if (!isShift)
                result = 'g';
            else
                result = 'G';
            break;
        case 0x23:
            if (!isShift)
                result = 'h';
            else
                result = 'H';
            break;
        case 0x24:
            if (!isShift)
                result = 'j';
            else
                result = 'J';
            break;
        case 0x25:
            if (!isShift)
                result = 'k';
            else
                result = 'K';
            break;
        case 0x26:
            if (!isShift)
                result = 'l';
            else
                result = 'L';
            break;
        case 0x27:
            if (!isShift)
                result = ';';
            else
                result = ':';
            break;

        case 0x2C:
            if (!isShift)
                result = 'z';
            else
                result = 'Z';
            break;
        case 0x2D:
            if (!isShift)
                result = 'x';
            else
                result = 'X';
            break;
        case 0x2E:
            if (!isShift)
                result = 'c';
            else
                result = 'C';
            break;
        case 0x2F:
            if (!isShift)
                result = 'v';
            else
                result = 'V';
            break;
        case 0x30:
            if (!isShift)
                result = 'b';
            else
                result = 'B';
            break;
        case 0x31:
            if (!isShift)
                result = 'n';
            else
                result = 'N';
            break;
        case 0x32:
            if (!isShift)
                result = 'm';
            else
                result = 'M';
            break;
        case 0x33:
            if (!isShift)
                result = ',';
            else
                result = '<';
            break;
        case 0x34:
            if (!isShift)
                result = '.';
            else
                result = '>';
            break;
        case 0x35:
            if (!isShift)
                result = '/';
            else
                result = '?';
            break;
        case 0x39:
            if (!isShift)
                result = ' ';
            else
                result = ' ';
            break;
        case 0x2B:
            if (!isShift)
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
        case 0x29:if (!isShift) printf("`"); else printf("~"); break;
        case 0x02:if (!isShift) printf("1"); else printf("!"); break;
        case 0x03:if (!isShift) printf("2"); else printf("@"); break;
        case 0x04:if (!isShift) printf("3"); else printf("#"); break;
        case 0x05:if (!isShift) printf("4"); else printf("$"); break;
        case 0x06:if (!isShift) printf("5"); else printf("%%"); break;
        case 0x07:if (!isShift) printf("6"); else printf("^"); break;
        case 0x08:if (!isShift) printf("7"); else printf("&"); break;
        case 0x09:if (!isShift) printf("8"); else printf("*"); break;
        case 0x0A:if (!isShift) printf("9"); else printf("("); break;
        case 0x0B:if (!isShift) printf("0"); else printf(")"); break;
        case 0x0C:if (!isShift) printf("-"); else printf("_"); break;
        case 0x0D:if (!isShift) printf("="); else printf("+"); break;
        case 0x0F: printf("\t"); break;
        case 0x10:if (!isShift) printf("q"); else printf("Q"); break;
        case 0x11:if (!isShift) printf("w"); else printf("W"); break;
        case 0x12:if (!isShift) printf("e"); else printf("E"); break;
        case 0x13:if (!isShift) printf("r"); else printf("R"); break;
        case 0x14:if (!isShift) printf("t"); else printf("T"); break;
        case 0x15:if (!isShift) printf("y"); else printf("Y"); break;
        case 0x16:if (!isShift) printf("u"); else printf("U"); break;
        case 0x17:if (!isShift) printf("i"); else printf("I"); break;
        case 0x18:if (!isShift) printf("o"); else printf("O"); break;
        case 0x19:if (!isShift) printf("p"); else printf("P"); break;
        case 0x1A:if (!isShift) printf("["); else printf("{"); break;
        case 0x1B:if (!isShift) printf("]"); else printf("}"); break;
        case 0x1C: 
            shell_buffer[shell_buffer_index] = '\0';
            shell_interpreter();
        break; //Enter
        case 0x1E:if (!isShift) printf("a"); else printf("A"); break;
        case 0x1F:if (!isShift) printf("s"); else printf("S"); break;
        case 0x20:if (!isShift) printf("d"); else printf("D"); break;
        case 0x21:if (!isShift) printf("f"); else printf("F"); break;
        case 0x22:if (!isShift) printf("g"); else printf("G"); break;
        case 0x23:if (!isShift) printf("h"); else printf("H"); break;
        case 0x24:if (!isShift) printf("j"); else printf("J"); break;
        case 0x25:if (!isShift) printf("k"); else printf("K"); break;
        case 0x26:if (!isShift) printf("l"); else printf("L"); break;
        case 0x27:if (!isShift) printf(";"); else printf(":"); break;
        case 0x28:if (!isShift) printf("E"); else printf("e"); break;
        case 0x2B:if (!isShift) printf("\\"); else printf("|"); break;
        case 0x2A: isShift = true; break;
        case 0x2C:if (!isShift) printf("z"); else printf("Z"); break;
        case 0x2D:if (!isShift) printf("x"); else printf("X"); break;
        case 0x2E: if (!isShift) printf("c"); else printf("C");break;
        case 0x2F:if (!isShift) printf("v"); else printf("V"); break;
        case 0x30:if (!isShift) printf("b"); else printf("B"); break;
        case 0x31:if (!isShift) printf("n"); else printf("N"); break;
        case 0x32:if (!isShift) printf("m"); else printf("M"); break;
        case 0x33:if (!isShift) printf(","); else printf("<"); break;
        case 0x34:if (!isShift) printf("."); else printf(">"); break;
        case 0x35:if (!isShift) printf("/"); else printf("?"); break;
        case 0x39: printf(" "); break;
        case 0x36: isShift = true; break;
        case 0xAA: case 0xB6: isShift = false; break;
        case 0x3A: if(isShift){
            isShift = false;
        }
        else{
            isShift = true;
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