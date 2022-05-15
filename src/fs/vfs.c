#include "vfs.h"

gentree_t *vfs_tree;
vfs_node* vfs_root;

uint32_t VFS_getFileSize(vfs_node* node)
{
    if (node && node->get_filesize)
    {
        return node->get_filesize(node);
    }
    return 0;
}

void VFS_db_listdir(char *name)
{
    vfs_node *n = file_open(name, 0);
    if (!n)
    {
        printf("Could not list a directory that does not exist\n");
        return;
    }
    if (!n->listdir)
        return;
    char **files = n->listdir(n);
    char **save = files;
    while (*files)
    {
        printf("%s ", *files);
        kfree(*files);
        files++;
    }
    kfree(save);
    printf("\n");
}

void print_vfstree_recur(gentree_node_t *node, int parent_offset)
{
    if (!node)
        return;
    char *tmp = (char*)kmalloc(512);
    int len = 0;
    memset(tmp, 0, 512);
    for (uint32_t i = 0; i < parent_offset; ++i)
    {
        strcat(tmp, " ");
    }
    char *curr = tmp + strlen(tmp);
    struct vfs_entry *fnode = (struct vfs_entry *)node->data;
    if (fnode->file)
    {
        printf(curr, "%s(0x%x, %s)", fnode->name, (uint32_t)fnode->file, fnode->file->name);
    }
    else
    {
        printf(curr, "%s(empty)", fnode->name);
    }
    printf("%s\n", tmp);
    len = strlen(fnode->name);
    kfree(tmp);
    foreach (child, node->children)
    {
        print_vfstree_recur((gentree_node_t*)child->val, parent_offset + len + 1);
    }
}

void print_vfs_tree()
{
    print_vfstree_recur(vfs_tree->root, 0);
}

uint32_t VFS_read(vfs_node *node, uint32_t offset, uint32_t size, char *buffer)
{
    if (node && node->read)
    {
        uint32_t ret = node->read(node, offset, size, buffer);
        return ret;
    }
    return -1;
}

uint32_t VFS_write(vfs_node *file, uint32_t offset, uint32_t size, char *buffer)
{
    if (file && file->write)
    {
        uint32_t ret = file->write(file, offset, size, buffer);
        return ret;
    }
    return -1;
}

void VFS_open(struct vfs_node *node, uint32_t flags)
{
    if (!node)
    {
        printf("Could not open a file that does not exist\n");
        return;
    }
    if (node->ref_count >= 0)
        node->ref_count++;
    node->open(node, flags);
}

void VFS_close(vfs_node *node)
{
    if (!node || node == vfs_root || node->ref_count == -1)
        return;
    node->ref_count--;
    if (node->ref_count == 0)
        node->close(node);
}

void VFS_chmod(vfs_node *node, uint32_t mode)
{
    if (node->chmod)
        return node->chmod(node, mode);
}

DirectoryEntry *VFS_readdir(vfs_node *node, unsigned int index)
{
    if (node && (node->flags & FS_DIRECTORY) && node->readdir)
        return node->readdir(node, index);
    return NULL;
}

vfs_node *VFS_finddir(vfs_node *node, char *name)
{
    if (node && (node->flags & FS_DIRECTORY) && node->finddir)
        return node->finddir(node, name);
    return NULL;
}

void VFS_mkdir(char *name, unsigned short permission)
{
    int i = strlen(name);
    char *dirname = strdup(name);
    char *save_dirname = dirname;
    char *parent_path = PATH_SEPARATOR_STRING;
    while (i >= 0)
    {
        if (dirname[i] == '/')
        {
            if (i != 0)
            {
                dirname[i] = '\0';
                parent_path = dirname;
            }
            dirname = &dirname[i + 1];
            break;
        }
        i--;
    }

    vfs_node *parent_node = file_open(parent_path, 0);
    if (!parent_node)
    {
        kfree(save_dirname);
    }

    if (parent_node->mkdir)
        parent_node->mkdir(parent_node, dirname, permission);
    kfree(save_dirname);

    VFS_close(parent_node);
}

int VFS_create(char *name, unsigned short permission)
{
    int i = strlen(name);
    char *dirname = strdup(name);
    char *save_dirname = dirname;
    char *parent_path = PATH_SEPARATOR_STRING;
    while (i >= 0)
    {
        if (dirname[i] == '/')
        {
            if (i != 0)
            {
                dirname[i] = '\0';
                parent_path = dirname;
            }
            dirname = &dirname[i + 1];
            break;
        }
        i--;
    }

    vfs_node *parent_node = file_open(parent_path, 0);
    if (!parent_node)
    {
        kfree(save_dirname);
        return -1;
    }
    if (parent_node->create)
        parent_node->create(parent_node, dirname, permission);
    kfree(save_dirname);
    VFS_close(parent_node);
    return 0;
}

void VFS_unlink(char *name)
{
    int i = strlen(name);
    char *dirname = strdup(name);
    char *save_dirname = dirname;
    char *parent_path = PATH_SEPARATOR_STRING;
    while (i >= 0)
    {
        if (dirname[i] == '/')
        {
            if (i != 0)
            {
                dirname[i] = '\0';
                parent_path = dirname;
            }
            dirname = &dirname[i + 1];
            break;
        }
        i--;
    }

    vfs_node *parent_node = file_open(parent_path, 0);
    if (!parent_node)
    {
        kfree(save_dirname);
    }
    if (parent_node->unlink)
        parent_node->unlink(parent_node, dirname);
    kfree(save_dirname);
    VFS_close(parent_node);
}

char *expand_path(char *input)
{
    list_t *input_list = str_split(input, PATH_SEPARATOR_STRING, NULL);
    char *ret = list2str(input_list, PATH_SEPARATOR_STRING);
    return ret;
}

vfs_node *get_mountpoint_recur(char **path, gentree_node_t *subroot)
{
    int found = 0;
    char *curr_token = strsep(path, PATH_SEPARATOR_STRING);
    if (curr_token == NULL || !strcmp(curr_token, ""))
    {
        struct vfs_entry *ent = (struct vfs_entry *)subroot->data;
        return ent->file;
    }
    foreach (child, subroot->children)
    {
        gentree_node_t *tchild = (gentree_node_t *)child->val;
        struct vfs_entry *ent = (struct vfs_entry *)(tchild->data);
        if (strcmp(ent->name, curr_token) == 0)
        {
            found = 1;
            subroot = tchild;
            break;
        }
    }

    if (!found)
    {
        *path = curr_token;
        return ((struct vfs_entry *)(subroot->data))->file;
    }
    return get_mountpoint_recur(path, subroot);
}

vfs_node *get_mountpoint(char **path)
{
    if (strlen(*path) > 1 && (*path)[strlen(*path) - 1] == '/')
        *(path)[strlen(*path) - 1] = '\0';
    if (!*path || *(path)[0] != '/')
        return NULL;
    if (strlen(*path) == 1)
    {
        *path = "\0";
        vfs_entry *ent = (vfs_entry *)vfs_tree->root->data;
        return ent->file;
    }
    (*path)++;
    return get_mountpoint_recur(path, vfs_tree->root);
}


vfs_node *file_open(char *file_name, uint32_t flags)
{
    char *curr_token = NULL;
    char *filename = strdup(file_name);
    char *free_filename = filename;
    char *save = strdup(filename);
    char *original_filename = filename;
    char *new_start = NULL;
    vfs_node *nextnode = NULL;
    vfs_node *startpoint = get_mountpoint(&filename);
    if (!startpoint)
        return NULL;
    if (filename)
        new_start = strstr(save + (filename - original_filename), filename);
    while (filename != NULL && ((curr_token = strsep(&new_start, PATH_SEPARATOR_STRING)) != NULL))
    {
        nextnode = VFS_finddir(startpoint, curr_token);
        if (!nextnode)
            return NULL;
        startpoint = nextnode;
    }
    if (!nextnode)
        nextnode = startpoint;
    VFS_open(nextnode, flags);
    kfree(save);
    kfree(free_filename);
    return nextnode;
}

void VFS_init()
{
    vfs_tree = gentree_create();
    struct vfs_entry *root = (vfs_entry*)kmalloc(sizeof(struct vfs_entry));
    root->name = strdup("root");
    root->file = NULL;
    gentree_insert(vfs_tree, NULL, root);
}

void VFS_mountDev(char *mountpoint, vfs_node *node)
{
    VFS_mount(mountpoint, node);
}

void VFS_mount_recur(char *path, gentree_node_t *subroot, vfs_node *fs_obj)
{
    int found = 0;
    char *curr_token = strsep(&path, PATH_SEPARATOR_STRING);

    if (curr_token == NULL || !strcmp(curr_token, ""))
    {
        struct vfs_entry *ent = (struct vfs_entry *)subroot->data;
        if (ent->file)
        {
            printf("A Device is already mounted at the path. Please unmount and try again\n");
            return;
        }
        if (!strcmp(ent->name, PATH_SEPARATOR_STRING))
            vfs_root = fs_obj;
        ent->file = fs_obj;
        return;
    }

    foreach (child, subroot->children)
    {
        gentree_node_t *tchild = (gentree_node_t *)child->val;
        struct vfs_entry *ent = (struct vfs_entry *)(tchild->data);
        if (strcmp(ent->name, curr_token) == 0)
        {
            found = 1;
            subroot = tchild;
        }
    }

    if (!found)
    {
        vfs_entry *ent = (vfs_entry*)kcalloc(1, sizeof(vfs_entry));
        ent->name = strdup(curr_token);
        subroot = gentree_insert(vfs_tree, subroot, ent);
    }
    VFS_mount_recur(path, subroot, fs_obj);
}

void VFS_mount(char *path, vfs_node *fs_obj)
{
    fs_obj->ref_count = -1;
    fs_obj->fs_type = 0;
    if (path[0] == '/' && strlen(path) == 1)
    {
        struct vfs_entry *ent = (struct vfs_entry *)vfs_tree->root->data;
        if (ent->file)
        {
            printf("A Device is already mounted at the path. Please unmount and try again\n");
            return;
        }
        vfs_root = fs_obj;
        ent->file = fs_obj;
        return;
    }
    VFS_mount_recur(path + 1, vfs_tree->root, fs_obj);
}