#include "initfile.h"

void load_initfile(void)
{
    list_t *initfile_list = list_create();

    FILE* f = file_open(INITFILE_PATH, 0);
    if(f == NULL)
    {
        printf("[initfile] Could not open initfile: %s\n", INITFILE_PATH);
        return;
    }

    char * buf = kmalloc(VFS_getFileSize(f));
    VFS_read(f, 0, VFS_getFileSize(f), buf);
    VFS_close(f);

    uint32_t n;

    initfile_list = str_split(buf, "\n", &n);

    foreach(v, initfile_list)
    {
        list_t* args = str_split(v->val, " ", &n);

        char* cmd = list_peek_front(args);

        if(strcmp(cmd, "video") == 0)
        {
            char* mode = list_pop(args)->val;
            list_t* args2 = str_split(mode, "x", &n);
            printf("[initfile] Setting video mode to %s\n", mode);
            uint32_t bpp = atoi(list_pop(args2)->val);
            uint32_t height = atoi(list_pop(args2)->val);
            uint32_t width = atoi(list_pop(args2)->val);

            init_vesa();
            vesa_change_mode(width, height, bpp);
        }
        else if(strcmp(cmd, "EXT2") == 0)
        {
            char* mount_point = list_pop(args)->val;
            char* path = list_pop(args)->val;
            EXT2_init(path, mount_point);
        }
        else if(strcmp(cmd, "BMP") == 0)
        {
            char* path = list_pop(args)->val;

            bitmap_display(path);
        }
        else if(strcmp(cmd, "TGA") == 0)
        {
            char* path = list_pop(args)->val;

            targa_display(path);
        }
        else if(strcmp(cmd, "echo") == 0)
        {
            listnode_t* l = args->head;
            printf("[InitFile] ");
            for(listnode_t* v2 = l->next; v2 != NULL; v2 = v2->next)
            {
                printf("%s ", v2->val);
            }
            printf("\n");
        }
        else if(strcmp(cmd, "#") == 0)
        {
            continue;
        }
        else
        {
            printf("[initfile] Unknown command: %s\n", cmd);
        }
    }
}