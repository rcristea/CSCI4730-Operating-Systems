#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "API.h"

int dir_make(char* name)
{
	int inodeNum = search_cur_dir(name);
	int block = get_block();
	if (inodeNum >= 0) {
		printf("mkdir Error: %s already exists.\n", name);
		return -1;
	} else if (curDir.numEntry + 1 > MAX_DIR_ENTRY) {
		printf("mkdir Error: Directory is full.\n");
		return -1;
	} else if (superBlock.freeInodeCount < 1) {
		printf("mkdir Error: Inode is full.\n");
		return -1;
	} else if (block < 0) {
		printf("mkdir Error: No free blocks.\n");
	}

	inodeNum = get_inode();
	if (inodeNum < 0) {
		printf("mkdir Error: No free inodes.\n");
		return -1;
	}

	Inode inode;
	inode.type = directory;
	inode.size = 1;
	inode.blockCount = 1;
	inode.directBlock[0] = block;
	write_inode(inodeNum, inode);

	strncpy(curDir.dentry[curDir.numEntry].name, name, strlen(name));
	curDir.dentry[curDir.numEntry].name[strlen(name)] = '\0';
	curDir.dentry[curDir.numEntry].inode = inodeNum;
	curDir.numEntry++;

	Dentry newDentry;
	newDentry.numEntry = 2;
	strncpy(newDentry.dentry[0].name, ".", 1);
	newDentry.dentry[0].name[1] = '\0';
	newDentry.dentry[0].inode = inodeNum;

	strncpy(newDentry.dentry[1].name, "..", 2);
	newDentry.dentry[1].name[2] = '\0';
	newDentry.dentry[1].inode = curDir.dentry[0].inode;

	write_disk_block(block, (char *)&newDentry);
	write_disk_block(curDirInode, (char*)&curDir);

	return 0;
}

int dir_remove(char *name)
{
	int inodeNum = search_cur_dir(name);
	Inode inode = read_inode(inodeNum);
	Dentry dentry;
	read_disk_block(inode.directBlock[0], (char *)&dentry);

	if (inodeNum < 0) {
		printf("rmdir Error: %s does not exist.\n", name);
		return -1;
	} else if (inode.type != directory) {
		printf("rmdir Error: %s is not a directory.\n", name);
		return -1;
	} else if (strcmp(".", name) == 0) {
		printf("rmdir Error: Cannot remove the current directory.\n");
		return -1;
	} else if (strcmp("..", name) == 0) {
		printf("rmdir Error: Cannot remove the parent directory.\n");
		return -1;
	} else if (dentry.numEntry > 2) {
		printf("rmdir Error: Directory is not empty.\n");
		return -1;
	}

	for (int i = 0; i < curDir.numEntry; i++) {
		if (strcmp(name, curDir.dentry[i].name) == 0) {
			for (int j = i + 1; i < curDir.numEntry; j++) {
				curDir.dentry[i] = curDir.dentry[j];
				i++;
			}

			curDir.numEntry--;
			break;
		}
	}

	for (int i = 0; i < inode.blockCount; i++) {
		free_block(inode.directBlock[i]);
	}

	if (inode.size > 7680) {
		free_block(inode.indirectBlock);
	}

	free_inode(inodeNum);

	return 0;
}

int dir_change(char* name)
{
	if (strcmp(name, ".") == 0) {
		return 0;
	} else if (strcmp(name, "..") == 0) {
		int parentInodeNum = curDir.dentry[1].inode;
		Inode parentInode = read_inode(parentInodeNum);
		int parentBlock = parentInode.directBlock[0];

		Inode currentInode = read_inode(curDirInode);
		write_disk_block(currentInode.directBlock[0], (char *)&curDir);

		read_disk_block(parentBlock, (char *)&curDir);
		curDirInode = parentInodeNum;

		return 0;
	} else {
		int inodeNum = search_cur_dir(name);
		Inode inode = read_inode(inodeNum);
		int inodeBlock = inode.directBlock[0];

    if (inodeNum < 0) {
      printf("cd Error: %s does not exist.\n", name);
      return -1;
    }

		Inode currentInode = read_inode(curDirInode);
		write_disk_block(currentInode.directBlock[0], (char *)&curDir);

		read_disk_block(inodeBlock, (char *)&curDir);
		curDirInode = inodeNum;

		return 0;
	}
}


/* ===================================================== */

int ls()
{
		int i;
		int inodeNum;
		Inode targetInode;
		for(i = 0; i < curDir.numEntry; i++)
		{
				inodeNum = curDir.dentry[i].inode;
				targetInode = read_inode(inodeNum);
				if(targetInode.type == file) printf("type: file, ");
				else printf("type: dir, ");
				printf("name \"%s\", inode %d, size %d byte\n", curDir.dentry[i].name, curDir.dentry[i].inode, targetInode.size);
		}

		return 0;
}


