#ifndef VFS_H
#define VFS_H

#include <stdint.h>
#include <stddef.h>

#define VFS_MAX_NODE_NAME_LEN 256
#define VFS_MAX_MOUNT_POINT_NAME_LEN 128
#define VFS_MAX_PATH_LEN 256

struct vfs_node_struct;
struct vfs_mount_point_struct;

struct dirent {
	char d_name[VFS_MAX_PATH_LEN];
};

typedef struct vfs_node_struct {
	void *private_inode;
	struct vfs_mount_point_struct *mount_point;
	uint64_t (* read)(struct vfs_node_struct *,void *buf,uint64_t off,size_t count);
	uint64_t (* write)(struct vfs_node_struct *,void *buf,uint64_t off,size_t count);
	int (* close)(struct vfs_node_struct *);
	struct vfs_node_struct *(* finddir)(struct vfs_node_struct *,char *name);
	int (* create)(struct vfs_node_struct*,char *name,int mode);
	int (* mkdir)(struct vfs_node_struct*,char *name,int mode);
	int (* unlink)(struct vfs_node_struct*,char *);
	struct dirent *(* readdir)(struct vfs_node_struct*,uint64_t index);
}vfs_node;

typedef struct vfs_mount_point_struct{
	char name[VFS_MAX_MOUNT_POINT_NAME_LEN];
	struct vfs_mount_point_struct *prev;
	struct vfs_mount_point_struct *next;
	vfs_node *root;
}vfs_mount_point;


struct kernel_table_struct;
void init_vfs(void);

int vfs_mount(const char *name,vfs_node *mounting_root);

vfs_node *vfs_open(const char *path);
vfs_node *vfs_finddir(vfs_node *node,const char *name);
uint64_t vfs_read(vfs_node *node,const void *buffer,uint64_t offset,size_t count);
uint64_t vfs_write(vfs_node *node,void *buffer,uint64_t offset,size_t count);
int vfs_create(vfs_node *node,const char *name,int perm);
int vfs_mkdir(vfs_node *node,const char *name,int perm);
void vfs_close(vfs_node *node);
int vfs_unlink(vfs_node *node,const char *name);
struct dirent *vfs_readdir(vfs_node *node,uint64_t index);
#endif