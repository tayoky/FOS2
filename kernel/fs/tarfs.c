#include "tarfs.h"
#include "kernel.h"
#include "string.h"
#include "tmpfs.h"
#include "print.h"
#include "asm.h"

static uint64_t octal2int(const char *octal){
	uint64_t integer = 0;
	while(*octal){
		integer *= 8;
		integer += (*octal) - '0';
		octal++;
	}
	return integer;
}

static void ls(const char *path){
	vfs_node *node = vfs_open(path);

	struct dirent *ret;
	uint64_t index = 0;
	while(1){
		ret = vfs_readdir(node,index);
		if(!ret)break;
		kprintf("%s\n",ret->d_name);
		index++;
	}

	vfs_close(node);
}

void mount_initrd(void){
	kstatus("unpack initrd ...");
	
	//create an tmpfs for it
	if(vfs_mount("initrd",new_tmpfs())){
		kfail();
		halt();
	}

	char *addr = (char *)kernel->initrd->address;

	//for each file in the tar file create one on the tmpfs
	while(!memcmp(((ustar_header *)addr)->ustar,"ustar",5)){
		ustar_header *current_file = (ustar_header *)addr;

		//find the full path of the file
		char *full_path = kmalloc(strlen(current_file->name) + strlen("initrd:/") + 1);
		strcpy(full_path,"initrd:/");
		strcat(full_path,current_file->name);

		//find file size
		uint64_t file_size = octal2int(current_file->file_size);

		if(current_file->type == USTAR_DIRTYPE){
			full_path[strlen(full_path)-1] = '\0';

			//create the directory
			if(vfs_mkdir(full_path,777)<0){
				kfail();
				kinfof("can't create folder %s for initrd\n",full_path);
				halt();
			}
		} else {
			//create the file
			if(vfs_create(full_path,777,VFS_FILE)){
				kfail();
				kinfof("fail to create file initrd:/%s\n",current_file->name);
				halt();
			}

			//open the file
			vfs_node *file = vfs_open(full_path);
			if(!file){
				kfail();
				kinfof("fail to open file : %s\n",full_path);
				halt();
			}

			//copy the files content
			int64_t write_size = vfs_write(file,addr + 512,0,file_size);
			if((uint64_t)write_size != file_size){
				kfail();
				kinfof("fail to write to file %s, can only write %luKB/%luKB\n",write_size,full_path,file_size);
				halt();
			}

			//now close and free
			vfs_close(file);
		};
		kfree(full_path);

		addr += (((uint64_t)file_size + 1023) / 512) * 512;
	}
	kok();
	ls("initrd:/");
}