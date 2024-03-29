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

#include "initfile.h"
#include "dhcp.h"

void load_init(char* path)
{
    list_t *initfile_list = list_create();

    FILE* f = file_open(path, 0);
    if(f == NULL)
    {
        printf("[initfile] Could not open initfile: %s\n", path);
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
        else if(strcmp(cmd, "mount") == 0)
        {
            char* mount_point = list_pop(args)->val;
            char* path = list_pop(args)->val;
            char* arg1 = list_pop(args)->val;

            printf("[initfile] Mounting %s at %s\n", path, mount_point);

            if(strcmp(arg1, "EXT2") == 0)
            {
                EXT2_init(path, mount_point);
            }
            else if(strcmp(arg1, "SORFS") == 0)
            {
                init_sorfs(path, mount_point);
            }
            else if(strcmp(arg1, "DEVFS") == 0)
            {
                devfs_mount(mount_point);
            }
            else if(strcmp(arg1, "PROCFS") == 0)
            {
                procfs_mount(mount_point);
            }
            else
            {
                printf("[initfile] Unknown mount type: %s\n", arg1);
            }
        }
        else if(strcmp(cmd, "umount") == 0)
        {
            char* path = list_pop(args)->val;
            VFS_unmount(path);
        }
        else if(strcmp(cmd, "read") == 0)
        {
            char* path = list_pop(args)->val;
            FILE* f = file_open(path, 0);
            if(f == NULL)
            {
                printf("[initfile] Could not open file: %s\n", path);
                continue;
            }
            uint32_t size = VFS_getFileSize(f);
            char* buf = kmalloc(size);
            VFS_read(f, 0, size, buf);
            VFS_close(f);
            printf("%s", buf);
            kfree(buf);
        }
        else if(strcmp(cmd, "load") == 0)
        {
            char* path = list_pop(args)->val;
            char* set = list_pop(args)->val;

            if(strcmp(set, "one"))
            {
                load_init(path);
            }

            if(strcmp(set, "all") == 0)
            {
                char** list = VFS_listdir(path);

                int i = 0;

                for(i = 0; list[i] != NULL; i++)
                {
                    char* file = kcalloc(strlen(path) + strlen(list[i]) + 2, 1);
                    if(strcmp(list[i], ".") == 0 || strcmp(list[i], "..") == 0)
                    {
                        continue;
                    }
                    sprintf(file, "%s/%s", path, list[i]);
                    load_init(file);
                }
            }
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

void load_initfile(void)
{
    int isFork;

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

            vesa_change_mode(width, height, bpp);

        }
        else if(strcmp(cmd, "EXT2") == 0)
        {
            char* mount_point = list_pop(args)->val;
            char* path = list_pop(args)->val;
            EXT2_init(path, mount_point);
        }
        else if(strcmp(cmd, "mount") == 0)
        {
            char* mount_point = list_pop(args)->val;
            char* path = list_pop(args)->val;
            char* arg1 = list_pop(args)->val;

            printf("[initfile] Mounting %s at %s\n", path, mount_point);

            if(strcmp(arg1, "EXT2") == 0)
            {
                EXT2_init(path, mount_point);
            }
            else if(strcmp(arg1, "SORFS") == 0)
            {
                init_sorfs(path, mount_point);
            }
            else if(strcmp(arg1, "DEVFS") == 0)
            {
                devfs_mount(mount_point);
            }
            else if(strcmp(arg1, "PROCFS") == 0)
            {
                procfs_mount(mount_point);
            }
            else
            {
                mount(path, mount_point);
            }
        }
        else if(strcmp(cmd, "umount") == 0)
        {
            char* path = list_pop(args)->val;
            VFS_unmount(path);
        }
        else if(strcmp(cmd, "hostname") == 0)
        {
            char* hostname = list_pop(args)->val;
            setHostName(hostname);
        }
        else if(strcmp(cmd, "ip") == 0)
        {
            uint8_t ip[4];
            ip[3] = atoi(list_pop(args)->val);
            ip[2] = atoi(list_pop(args)->val);
            ip[1] = atoi(list_pop(args)->val);
            ip[0] = atoi(list_pop(args)->val);
            char* arg1 = list_pop(args)->val;

            if(strcmp(arg1, "dhcp") == 0)
            {
                dhcp_discover();
            }
            else if(strcmp(arg1, "static") == 0)
            {
                setStaticIP(ip);
            }
        }
        else if(strcmp(cmd, "netinterface") == 0)
        {
            char* arg1 = list_pop(args)->val;

            if(strcmp(arg1, "on") == 0)
            {
                changeNetworkState(true);
            }
            else if(strcmp(arg1, "off") == 0)
            {
                changeNetworkState(false);
            }
            else
            {
                printf("[init] Invalid option %s for netinterface command!\n", arg1);
                printl("[init] Invalid option %s for netinterface command!\n", arg1);
            }
        }
        else if(strcmp(cmd, "udpsend") == 0)
        {
            char* payload = list_pop(args)->val;
            payload[strlen(payload)] = '\0';
            char* dstIP = list_pop(args)->val;
            list_t* ip_list = str_split(dstIP, ".", NULL);
            uint16_t dstPort = atoi(list_pop(args)->val);
            uint16_t srcPort = atoi(list_pop(args)->val);

            uint8_t ip[4];

            int i = 0;
            foreach(t, ip_list)
            {
                char* ipp = t->val;
                ip[i] = atoi(ipp);
                i++;
            }

            while(isIPReady() == 0);
            udp_send_packet(ip, srcPort, dstPort, payload, strlen(payload));
        }
        else if(strcmp(cmd, "ping") == 0)
        {
            char* dstIP = list_pop(args)->val;
            list_t* ip_list = str_split(dstIP, ".", NULL);

            uint8_t ip[4];

            int i = 0;
            foreach(t, ip_list)
            {
                char* ipp = t->val;
                ip[i] = atoi(ipp);
                i++;
            }

            while(isIPReady() == 0);

            request_echo_reply(ip);
        }
        else if(strcmp(cmd, "init") == 0)
        {
            char* arg1 = list_pop(args)->val;
            if(strcmp(arg1, "shell") == 0)
            {
                init_shell();
            }
            else if(strcmp(arg1, "arp") == 0)
            {
                init_arp();
            }
            else if(strcmp(arg1, "dhcp") == 0)
            {
                init_dhcp();
            }
            else if(strcmp(arg1, "udp") == 0)
            {
                init_udp();
            }
            else
            {
                printf("[INIT] unknown option %s.", arg1);
            }
        }
        else if(strcmp(cmd, "power") == 0)
        {
            char* arg1 = list_pop(args)->val;
            if(strcmp(arg1, "off") == 0)
            {
                apm_shutdown();
            }
            else if(strcmp(arg1, "reboot") == 0)
            {
                reboot();
            }
            else
            {
                reboot();
            }
        }
        else if(strcmp(cmd, "switch") == 0)
        {
            int interfacenum = atoi(list_pop(args)->val);
            switch_interface(interfacenum);
        }
        else if(strcmp(cmd, "read") == 0)
        {
            char* path = list_pop(args)->val;
            FILE* f = file_open(path, 0);
            if(f == NULL)
            {
                printf("[initfile] Could not open file: %s\n", path);
                continue;
            }
            uint32_t size = VFS_getFileSize(f);
            char* buf = kmalloc(size);
            VFS_read(f, 0, size, buf);
            VFS_close(f);
            printf("%s", buf);
            kfree(buf);
        }
        else if(strcmp(cmd, "load") == 0)
        {
            char* path = list_pop(args)->val;

            load_init(path);
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
        else if(strcmp(cmd, "execC") == 0)
        {
            char* path = list_pop(args)->val;
            printf("[InitFile] Executing %s...\n", path);

            if(isFork == 0)
            {
                asm volatile("int $0x80" :: "a"(0x08));
            }
            else
            {
                printf("Forked...\n");
            }
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