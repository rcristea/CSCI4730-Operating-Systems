#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "API.h"

char inodeMap[MAX_INODE / 8];
char blockMap[MAX_BLOCK / 8];
Inode inode[MAX_INODE];
SuperBlock superBlock;
Dentry curDir;
int curDirInode;

Inode read_inode(int inodeNum)
{
		return inode[inodeNum];
}

void write_inode(int inodeNum, Inode newInode)
{
		inode[inodeNum] = newInode;
}

int fs_mount(char *name)
{
		int numInodeBlock =  (sizeof(Inode)*MAX_INODE)/ BLOCK_SIZE;
		int i, index, inode_index = 0;

		// load superblock, inodeMap, blockMap and inodes into the memory
		if(disk_mount(name) == 1) {
				read_disk_block(0, (char*) &superBlock);
				if(superBlock.magicNumber != MAGIC_NUMBER) {
						printf("Invalid disk!\n");
						exit(0);
				}
				read_disk_block(1, inodeMap);
				read_disk_block(2, blockMap);
				for(i = 0; i < numInodeBlock; i++)
				{
						index = i+3;
						read_disk_block(index, (char*) (inode+inode_index));
						inode_index += (BLOCK_SIZE / sizeof(Inode));
				}
				// root directory
				curDirInode = inode[0].directBlock[0];
				read_disk_block(curDirInode, (char*)&curDir);

		} else {
				// Init file system superblock, inodeMap and blockMap
				superBlock.magicNumber = MAGIC_NUMBER;
				superBlock.freeBlockCount = MAX_BLOCK - (1+1+1+numInodeBlock);
				superBlock.freeInodeCount = MAX_INODE;
				superBlock.rootDirInode = 0;

				//Init inodeMap
				for(i = 0; i < MAX_INODE / 8; i++)
				{
						set_bit(inodeMap, i, 0);
				}
				//Init blockMap
				for(i = 0; i < MAX_BLOCK / 8; i++)
				{
						if(i < (1+1+1+numInodeBlock)) set_bit(blockMap, i, 1);
						else set_bit(blockMap, i, 0);
				}
				//Init root dir
				int rootInode = get_inode();
				curDirInode = get_block();

				inode[rootInode].type =directory;
				inode[rootInode].size = 1;
				inode[rootInode].blockCount = 1;
				inode[rootInode].directBlock[0] = curDirInode;

				curDir.numEntry = 1;
				strncpy(curDir.dentry[0].name, ".", 1);
				curDir.dentry[0].name[1] = '\0';
				curDir.dentry[0].inode = rootInode;
				write_disk_block(curDirInode, (char*)&curDir);
		}
		return 0;
}

int fs_umount(char *name)
{
		int numInodeBlock =  (sizeof(Inode)*MAX_INODE )/ BLOCK_SIZE;
		int i, index, inode_index = 0;
		write_disk_block(0, (char*) &superBlock);
		write_disk_block(1, inodeMap);
		write_disk_block(2, blockMap);
		for(i = 0; i < numInodeBlock; i++)
		{
				index = i+3;
				write_disk_block(index, (char*) (inode+inode_index));
				inode_index += (BLOCK_SIZE / sizeof(Inode));
		}
		// current directory
		write_disk_block(curDirInode, (char*)&curDir);

		disk_umount(name);	
}

int search_cur_dir(char *name)
{
		// return inode. If not exist, return -1
		int i;

		for(i = 0; i < curDir.numEntry; i++)
		{
				if(command(name, curDir.dentry[i].name)) return curDir.dentry[i].inode;
		}
		return -1;
}

int fs_stat()
{
		printf("File System Status: \n");
		printf("# of free blocks: %d (%d bytes), # of free inodes: %d\n", superBlock.freeBlockCount, superBlock.freeBlockCount*512, superBlock.freeInodeCount);
}

int execute_command(char *comm, char *arg1, char *arg2, int numArg)
{
		if(command(comm, "create")) {
				if(numArg < 2) {
						printf("error: create <filename> <size>\n");
						return -1;
				}
				return file_create(arg1, atoi(arg2)); // (filename, size)
		} else if(command(comm, "cat")) {
				if(numArg < 1) {
						printf("error: cat <filename>\n");
						return -1;
				}
				return file_cat(arg1); // file_cat(filename)
		} else if(command(comm, "rm")) {
				if(numArg < 1) {
						printf("error: rm <filename>\n");
						return -1;
				}
				return file_remove(arg1); //(filename)
		} else if(command(comm, "mkdir")) {
				if(numArg < 1) {
						printf("error: mkdir <dirname>\n");
						return -1;
				}
				return dir_make(arg1); // (dirname)
		} else if(command(comm, "rmdir")) {
				if(numArg < 1) {
						printf("error: rmdir <dirname>\n");
						return -1;
				}
				return dir_remove(arg1); // (dirname)
		} else if(command(comm, "cd")) {
				if(numArg < 1) {
						printf("error: cd <dirname>\n");
						return -1;
				}
				return dir_change(arg1); // (dirname)
		} else if(command(comm, "ls"))  {
				return ls();
		} else if(command(comm, "stat")) {
				if(numArg < 1) {
						printf("error: stat <filename>\n");
						return -1;
				}
				return file_stat(arg1); //(filename)
		} else if(command(comm, "df")) {
				return fs_stat();
		} else if(command(comm, "ln")) {
				if(numArg < 2) {
						printf("error: ln <src> <dest>\n");
						return -1;
				}
				return hard_link(arg1, arg2); // hard link. arg1: src file or dir, arg2: destination file or dir
		} else if(command(comm, "cp")) {
				if(numArg < 2) {
						printf("error: cp <src> <dest>\n");
						return -1;
				}
				return file_copy(arg1, arg2); // hard link. arg1: src file or dir, arg2: destination file or dir
		} else {
				fprintf(stderr, "%s: command not found.\n", comm);
				return -1;
		}
		return 0;
}

