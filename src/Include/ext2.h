#ifndef __EXT2_H
#define __EXT2_H

#include "system.h"
#include "string.h"
#include "printf.h"
#include "kheap.h"
#include "vfs.h"
#include "xxd.h"

#define EXT2_DIRECT_BLOCKS 12

#define SUPERBLOCK_SIZE 1024
#define ROOT_INODE_NUMBER 2

#define EXT2_IFSOCK 0xC000
#define EXT2_IFLNK 0xA000
#define EXT2_IFREG 0x8000
#define EXT2_IFBLK 0x6000
#define EXT2_IFDIR 0x4000
#define EXT2_IFCHR 0x2000
#define EXT2_IFIFO 0x1000

typedef struct superblock
{
    uint32_t total_inodes;
    uint32_t total_blocks;
    uint32_t reserved_blocks;
    uint32_t free_blocks;
    uint32_t free_inodes;
    uint32_t first_data_block;
    uint32_t log_block_size;
    uint32_t log_frag_size;
    uint32_t blocks_per_group;
    uint32_t frags_per_group;
    uint32_t inodes_per_group;

    uint32_t mtime;
    uint32_t wtime;

    uint16_t mnt_count;
    uint16_t max_mnt_count;
    uint16_t magic;
    uint16_t state;
    uint16_t errors;
    uint16_t minor_rev_level;
    uint32_t lastcheck;
    uint32_t checkinterval;
    uint32_t creator_os;
    uint32_t rev_level;

    uint16_t def_resuid;
    uint16_t def_resgid;

    // Extended Features
    uint32_t first_inode;
    uint16_t inode_size;

    uint16_t SBlock_Group;

    uint32_t opt_features;
    uint32_t req_features;
    uint32_t ro_features;

    char fs_id[16];
    char volume_name[16];
    char last_mounted[64];

    uint32_t compression;

    uint8_t prealloc_blocks;
    uint8_t prealloc_dir_blocks;

    uint16_t Unused1;

    uint8_t journal_id[16];
    uint32_t journal_inode;
    uint32_t journal_device;

    uint32_t orphan_head;

    char unused2[1024 - 236];
} __attribute__((packed))superblock;

typedef struct block_group_descriptor
{
    uint32_t block_bitmap;
    uint32_t inode_bitmap;
    uint32_t inode_table;
    uint16_t free_blocks_count;
    uint16_t free_inodes_count;
    uint16_t used_dirs_count;
    uint16_t pad;
    uint8_t reserved[12];
} __attribute__((packed))block_group_descriptor;

typedef struct Dirent
{
    uint32_t inode;
    uint16_t rec_len;
    uint8_t name_len;
    uint8_t file_type;
    uint8_t name[];
} __attribute__((packed)) Dirent;

typedef struct inode
{
    uint16_t permission;
    uint16_t uid;
    uint32_t size;
    uint32_t atime;
    uint32_t ctime;
    uint32_t mtime;
    uint32_t dtime;
    uint16_t gid;
    uint16_t hard_links;
    uint32_t num_sectors;
    uint32_t flags;
    uint32_t os_specific1;
    uint32_t blocks[EXT2_DIRECT_BLOCKS + 3];
    uint32_t generation;
    uint32_t file_acl;
    union
    {
        uint32_t dir_acl;
        uint32_t size_high;
    };
    uint32_t f_block_addr;
    char os_specific2[12];

} __attribute__((packed)) inode_t;

struct ext2_cache
{
    uint32_t block;
    uint32_t times;
    uint8_t dirty;
    char *block_data;
};

typedef struct ext2_fs
{
    vfs_node *device;

    superblock *sb;
    block_group_descriptor *bgd;
    uint32_t block_size;
    uint32_t sectors_per_block;
    uint32_t block_per_group;

    uint32_t inodes_per_group;
    uint32_t total_groups;

    uint32_t bgd_block;
}ext2_fs;

uint32_t EXT2_getFileSize(vfs_node* node);

void EXT2_mkdir(vfs_node* parent, char* name, uint16_t permission);
void EXT2_mkfile(vfs_node* parent, char* name, uint16_t permission);

void EXT2_createEntry(vfs_node* parent, char* name, uint32_t entry_inode);
void EXT2_removeEntry(vfs_node* parent, char* name);

void EXT2_unlink(vfs_node* parent, char* name);

char** EXT2_listdir(vfs_node* node);
vfs_node* EXT2_finddir(vfs_node* parent, char* name);

void EXT2_chmod(vfs_node* node, uint32_t mode);

uint32_t EXT2_read(vfs_node *file, uint32_t offset, uint32_t size, char *buffer);
uint32_t EXT2_write(vfs_node *file, uint32_t offset, uint32_t size, char *buffer);

void EXT2_open(vfs_node *file, uint32_t flags);
void EXT2_close(vfs_node* file);

void read_inode_metadata(ext2_fs* fs, inode_t* inode, uint32_t inode_num);
void write_inode_metadata(ext2_fs* fs, inode_t* inode, uint32_t inode_num);

uint32_t read_inode_filedata(ext2_fs* fs, inode_t* inode, uint32_t offset, uint32_t size, char* buffer);
void write_inode_filedata(ext2_fs* fs, inode_t* inode, uint32_t inode_num, uint32_t offset, uint32_t size, char* buffer);

char* read_inode_block(ext2_fs* fs, inode_t* inode, uint32_t iblock_num);
void write_inode_block(ext2_fs* fs, inode_t* inode, uint32_t iblock_num, char* buffer);

void read_disk_block(ext2_fs* fs, uint32_t block_num, char* buffer);
void write_disk_block(ext2_fs* fs, uint32_t block_num, char* buffer);

void rewrite_bgd(ext2_fs* fs);
void rewrite_superblock(ext2_fs* fs);

int alloc_inode_metadata_block(uint32_t *block_ptr, ext2_fs *ext2fs, inode_t* inode, uint32_t inode_num, char *buffer, unsigned int block_overwrite);

uint32_t get_DBlock_num(ext2_fs *fs, inode_t* inode, uint32_t IBlock_num);
void set_DBlock_num(ext2_fs *fs, inode_t* inode, uint32_t inode_num, uint32_t IBlock_num, uint32_t DBlock_num);

uint32_t EXT2_alloc_block(ext2_fs *fs);
void EXT2_free_block(ext2_fs *fs, uint32_t block_num);

void alloc_IBlock(ext2_fs* ext2fs, inode_t* inode, uint32_t inode_num, uint32_t block);
void free_IBlock(ext2_fs* ext2fs, inode_t* inode, uint32_t inode_num, uint32_t block);

uint32_t alloc_inode(ext2_fs* fs);
void free_inode(ext2_fs* fs, uint32_t inode_num);

vfs_node *EXT2_get_root(ext2_fs *fs, inode_t* inode);

void EXT2_init(char *dev_path, char *mountpoint);

void ext2_DebugPrintInode(inode_t* inode, ext2_fs* fs);
void ext2_DebugPrintBGD(ext2_fs* bgd);
void ext2_DebugPrintSB(ext2_fs* ext2fs);

#endif