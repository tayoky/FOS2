#ifndef VFS_H
#define VFS_H

#include <stdint.h>
#include <stddef.h>

#define VFS_MAX_NODE_NAME_LEN 256
#define VFS_MAX_MOUNT_POINT_NAME_LEN 128
#define VFS_MAX_PATH_LEN 256

struct vfs_node_struct;
struct vfs_mount_point_struct;

typedef uint64_t gid_t;
typedef uint64_t uid_t;
typedef int mode_t;

struct dirent {
	char d_name[VFS_MAX_PATH_LEN];
};

typedef struct vfs_node_struct {
	void *private_inode;
	struct vfs_mount_point_struct *mount_point;
	uid_t owner;
	gid_t group_owner;
	mode_t perm;
	uint64_t (* read)(struct vfs_node_struct *,void *buf,uint64_t off,size_t count);
	uint64_t (* write)(struct vfs_node_struct *,void *buf,uint64_t off,size_t count);
	int (* close)(struct vfs_node_struct *);
	struct vfs_node_struct *(* finddir)(struct vfs_node_struct *,char *name);
	int (* create)(struct vfs_node_struct*,char *name,mode_t perm);
	int (* mkdir)(struct vfs_node_struct*,char *name,mode_t perm);
	int (* unlink)(struct vfs_node_struct*,char *);
	struct dirent *(* readdir)(struct vfs_node_struct*,uint64_t index);
	int (* truncate)(struct vfs_node_struct*,size_t);
	int (* chmod)(struct vfs_node_struct*,mode_t perm);
	int (* chown)(struct vfs_node_struct*,uid_t owner,gid_t group_owner);
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

/// @brief open a context for a given path (absolute)
/// @param path 
/// @return an pointer to the vfs_node or NULL if fail
vfs_node *vfs_open(const char *path);
vfs_node *vfs_finddir(vfs_node *node,const char *name);
uint64_t vfs_read(vfs_node *node,const void *buffer,uint64_t offset,size_t count);
uint64_t vfs_write(vfs_node *node,void *buffer,uint64_t offset,size_t count);
int vfs_create(const char *path,int perm);
int vfs_mkdir(const char *path,int perm);

/// @brief close an context
/// @param node the context to close
void vfs_close(vfs_node *node);

/// @brief unlink a directory entry
/// @deprecated will likely change in future version
/// @param node 
/// @param name 
/// @return 
int vfs_unlink(vfs_node *node,const char *name);

/// @brief read an entry in a directory at a specifed index
/// @param node context of the dir
/// @param index the index to reads
/// @return a pointer to a dirent that can be free or NULL if fail
struct dirent *vfs_readdir(vfs_node *node,uint64_t index);

/// @brief truncate a file to a specfied size
/// @param node context of the file
/// @param size the new size
/// @return 0 on succes else error code
int vfs_truncate(vfs_node *node,size_t size);

/// @brief change permission of a file/dir
/// @param node context of the file/dir
/// @param perm new permission
/// @return 0 on succes else error code
int vfs_chmod(vfs_node *node,mode_t perm);

/// @brief change owner of a file/dir
/// @param node context for the file/dir
/// @param owner uid of new owner
/// @param group_owner gid of new group_owner
/// @return 0 on succes else error code
int vfs_chown(vfs_node *node,uid_t owner,gid_t group_owner);
#endif