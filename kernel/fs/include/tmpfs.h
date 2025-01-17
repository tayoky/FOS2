#ifndef TMPFS_H
#define TMPFS_H
#include "vfs.h"

void init_tmpfs();
vfs_node *new_tmpfs();

struct tmpfs_inode_struct;

typedef struct {
	char name[256];
	struct tmpfs_inode_struct *child;
	struct tmpfs_inode_struct *brother;
	uint64_t children_count;
	uint64_t flags;
	size_t buffer_size;
	char *buffer;
}tmpfs_inode;

vfs_node *tmpfs_finddir(vfs_node *node,const char *name);
uint64_t tmpfs_read(vfs_node *node,const void *buffer,uint64_t offset,size_t count);
uint64_t tmpfs_write(vfs_node *node,void *buffer,uint64_t offset,size_t count);
void tmpfs_close(vfs_node *node);
int tmpfs_create(vfs_node *node,const char *name,int perm);
int tmpfs_mkdir(vfs_node *node,const char *name,int perm);

#define TMPFS_FLAGS_FILE 0x01
#define TMPFS_FLAGS_DIR   0x02

#endif