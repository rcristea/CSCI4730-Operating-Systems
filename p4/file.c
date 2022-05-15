#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "API.h"

int file_cat(char *name)
{
	int inodeNum = search_cur_dir(name);
	Inode inode = read_inode(inodeNum);

	int numBlock = inode.size / BLOCK_SIZE;
	if (inode.size % BLOCK_SIZE > 0) {
		numBlock++;
	}

	if (inodeNum < 0) {
		printf("cat Error: File %s was not found.\n", name);
		return -1;
	} else if (inode.type == directory) {
		printf("cat Error: Cannot perform cat on a directory.\n");
		return -1;
	}

	char buffer[BLOCK_SIZE];
	int size = inode.size;
	char *str = (char *)malloc(sizeof(char) * (size + 1));
	str[size] = '\0';

	for (int i = 0; i < 15; i++) {
		if (i >= numBlock) {
			break;
		}

		read_disk_block(inode.directBlock[i], buffer);

		if (size >= BLOCK_SIZE) {
		  memcpy(str + i * BLOCK_SIZE, buffer, BLOCK_SIZE);
		  size -= BLOCK_SIZE;
		} else {
		  memcpy(str + i * BLOCK_SIZE, buffer, size);
		}
	}

	if (inode.size > 7680) {
		int indirectBlockMap[128];
		read_disk_block(inode.indirectBlock, (char *) indirectBlockMap);

		char indirectBuffer[BLOCK_SIZE];
		for (int i = 15; i < numBlock; i++) {
			read_disk_block(indirectBlockMap[i - 15], indirectBuffer);

			if (size >= BLOCK_SIZE) {
				memcpy(str + i * BLOCK_SIZE, indirectBuffer, BLOCK_SIZE);
				size -= BLOCK_SIZE;
			} else {
				memcpy(str + i * BLOCK_SIZE, indirectBuffer, size);
			}
		}
	}
	
	printf("%s\n", str);
	free(str);

	return 0;
}

int file_remove(char *name)
{
	int inodeNum = search_cur_dir(name);
	Inode inode = read_inode(inodeNum);

	if (inodeNum < 0) {
		printf("rm Error: File %s was not found.\n", name);
		return -1;
	} else if (inode.type == directory) {
		printf("rm Error: Cannot remove a directory.\n");
		return -1;
	} else if (curDir.numEntry == 0) {
		printf("rm Error: There are no files to remove in this directory\n");
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

	int numBlock = inode.size / BLOCK_SIZE;
	if (inode.size % BLOCK_SIZE > 0) {
		numBlock++;
	}

	if (inode.linkCount == 1) {
		for (int i = 0; i < 15; i++) {
			if (i >= numBlock) {
				break;
			}

			free_block(inode.directBlock[i]);
		}

		if (inode.size > 7680) {
			int indirectBuffer[128];
			read_disk_block(inode.indirectBlock, indirectBuffer);

			for (int i = 15; i < numBlock; i++) {
				free_block(indirectBuffer[i - 15]);
			}

			free_block(inode.indirectBlock);
		}

		free_inode(inodeNum);
	} else {
		inode.linkCount--;
		write_inode(inodeNum, inode);
	}

	return 0;
}

int hard_link(char *src, char *dest)
{
	int srcInodeNum = search_cur_dir(src);
	int destInodeNum = search_cur_dir(dest);
	Inode srcInode = read_inode(srcInodeNum);
	Inode destInode = read_inode(destInodeNum);

	if (srcInodeNum < 0) {
		printf("ln Error: File %s was not found.\n", src);
		return -1;
	} else if (destInodeNum >= 0) {
		printf("ln Error: File %s exists and cannot be used for linking.\n", dest);
		return -1;
	} else if (srcInode.type == directory) {
		printf("ln Error: %s is a directory.\n", src);
		return -1;
	} else if (curDir.numEntry + 1 > MAX_DIR_ENTRY) {
		printf("ln Error: Current directory is full.\n");
		return -1;
	} else if (srcInode.blockCount > superBlock.freeBlockCount) {
		printf("ln Error: Not enough free blocks in the super block.\n");
		return -1;
	} else if (superBlock.freeInodeCount < 1) {
		printf("ln Error: Not enough free inodes in the super block.\n");
		return -1;
	}

	srcInode.linkCount++;
	write_inode(srcInodeNum, srcInode);

	strncpy(curDir.dentry[curDir.numEntry].name, dest, strlen(dest));
	curDir.dentry[curDir.numEntry].name[strlen(dest)] = '\0';
	curDir.dentry[curDir.numEntry].inode = srcInodeNum;
	curDir.numEntry++;

	printf("Link created: %s\n", dest);

	return 0;
}

int file_copy(char *src, char *dest)
{
	int srcInodeNum = search_cur_dir(src);
	int destInodeNum = search_cur_dir(dest);
	Inode srcInode = read_inode(srcInodeNum);

	int numBlock = srcInode.size / BLOCK_SIZE;
	if (srcInode.size % BLOCK_SIZE > 0) {
		numBlock++;
	}

	if (srcInodeNum < 0) {
		printf("cp Error: File %s was not found.\n", src);
		return -1;
	} else if (destInodeNum >= 0) {
		printf("cp Error: File %s exists and cannot be over written.\n", dest);
		return -1;
	} else if (curDir.numEntry + 1 > MAX_DIR_ENTRY) {
		printf("cp Error: directory is full.\n");
		return -1;
	} else if (superBlock.freeInodeCount < 1) {
		printf("cp Error: inode is full.\n");
		return -1;
	} else if ((srcInode.size > 7680 && numBlock + 1 > superBlock.freeBlockCount) || numBlock > superBlock.freeBlockCount) {
		printf("cp Error: data block is full.\n");
		return -1;
	}

	destInodeNum = get_inode();
	if (destInodeNum < 0) {
		printf("cp Error: Not enough inodes.\n");
		return -1;
	}

	Inode destInode;
	destInode.type = srcInode.type;
	destInode.size = srcInode.size;
	destInode.blockCount = srcInode.blockCount;
	destInode.linkCount = 1;
	int block;
	char srcBuffer[BLOCK_SIZE];

	for (int i = 0; i < 15; i++) {
		if (i >= numBlock) {
			break;
		}

		block = get_block();
		if (block == -1) {
			printf("cp Error: get_block failed.\n");
			return -1;
		}

		read_disk_block(srcInode.directBlock[i], srcBuffer);
		destInode.directBlock[i] = block;
		write_disk_block(block, srcBuffer);
	}

	if (srcInode.size > 7680) {
		block = get_block();
		if (block == -1) {
			printf("cp Error: get_block failed.\n");
			return -1;
		}

		destInode.indirectBlock = block;

		int srcIndirectBlockMap[128];
		int destIndirectBlockMap[128];
		read_disk_block(srcInode.indirectBlock, (char *) srcIndirectBlockMap);
		printf("3\n");

		char srcIndirectBuffer[BLOCK_SIZE];
		for (int i = 15; i < numBlock; i++) {
			block = get_block();
			if (block == -1) {
				printf("cp Error: get_block failed.\n");
				return -1;
			}

			read_disk_block(srcIndirectBlockMap[i - 15], srcIndirectBuffer);
			destIndirectBlockMap[i - 15] = block;
			write_disk_block(block, srcIndirectBuffer);
		}

		write_disk_block(destInode.indirectBlock, (char *) destIndirectBlockMap);
	}

	strncpy(curDir.dentry[curDir.numEntry].name, dest, strlen(dest));
	curDir.dentry[curDir.numEntry].name[strlen(dest)] = '\0';
	curDir.dentry[curDir.numEntry].inode = destInodeNum;
	curDir.numEntry++;

	write_inode(destInodeNum, destInode);
	printf("File copy success: copied %s to %s.\n", src, dest);

	return 0;
}


/* =============================================================*/

int file_create(char *name, int size)
{
		int i;
		int block, inodeNum, numBlock;

		if(size <= 0 || size > 73216){
				printf("File create failed: file size error\n");
				return -1;
		}

		inodeNum = search_cur_dir(name); 
		if(inodeNum >= 0) {
				printf("File create failed:  %s exist.\n", name);
				return -1;
		}

		if(curDir.numEntry + 1 > MAX_DIR_ENTRY) {
				printf("File create failed: directory is full!\n");
				return -1;
		}

		if(superBlock.freeInodeCount < 1) {
				printf("File create failed: inode is full!\n");
				return -1;
		}

		numBlock = size / BLOCK_SIZE;
		if(size % BLOCK_SIZE > 0) numBlock++;

		if(size > 7680) {
				if(numBlock+1 > superBlock.freeBlockCount)
				{
						printf("File create failed: data block is full!\n");
						return -1;
				}
		} else {
				if(numBlock > superBlock.freeBlockCount) {
						printf("File create failed: data block is full!\n");
						return -1;
				}
		}

		char *tmp = (char*) malloc(sizeof(int) * size+1);

		rand_string(tmp, size);
		printf("File contents:\n%s\n", tmp);

		// get inode and fill it
		inodeNum = get_inode();
		if(inodeNum < 0) {
				printf("File_create error: not enough inode.\n");
				return -1;
		}
		
		Inode newInode;

		newInode.type = file;
		newInode.size = size;
		newInode.blockCount = numBlock;
		newInode.linkCount = 1;

		// add a new file into the current directory entry
		strncpy(curDir.dentry[curDir.numEntry].name, name, strlen(name));
		curDir.dentry[curDir.numEntry].name[strlen(name)] = '\0';
		curDir.dentry[curDir.numEntry].inode = inodeNum;
		curDir.numEntry++;

		// get data blocks
		for(i = 0; i < 15; i++)
		{
				if (i >= numBlock) break;
				block = get_block();
				if(block == -1) {
						printf("File_create error: get_block failed\n");
						return -1;
				}
				//set direct block
				newInode.directBlock[i] = block;

				write_disk_block(block, tmp+(i*BLOCK_SIZE));
		}

		if(size > 7680) {
				// get an indirect block
				block = get_block();
				if(block == -1) {
						printf("File_create error: get_block failed\n");
						return -1;
				}

				newInode.indirectBlock = block;
				int indirectBlockMap[128];

				for(i = 15; i < numBlock; i++)
				{
						block = get_block();
						if(block == -1) {
								printf("File_create error: get_block failed\n");
								return -1;
						}
						//set direct block
						indirectBlockMap[i-15] = block;
						write_disk_block(block, tmp+(i*BLOCK_SIZE));
				}
				write_disk_block(newInode.indirectBlock, (char*)indirectBlockMap);
		}

		write_inode(inodeNum, newInode);
		printf("File created. name: %s, inode: %d, size: %d\n", name, inodeNum, size);

		free(tmp);
		return 0;
}

int file_stat(char *name)
{
		char timebuf[28];
		Inode targetInode;
		int inodeNum;
		
		inodeNum = search_cur_dir(name);
		if(inodeNum < 0) {
				printf("file cat error: file is not exist.\n");
				return -1;
		}
		
		targetInode = read_inode(inodeNum);
		printf("Inode = %d\n", inodeNum);
		if(targetInode.type == file) printf("type = file\n");
		else printf("type = directory\n");
		printf("size = %d\n", targetInode.size);
		printf("linkCount = %d\n", targetInode.linkCount);
		printf("num of block = %d\n", targetInode.blockCount);
}


