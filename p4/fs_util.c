#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "fs.h"

int rand_string(char *str, size_t size)
{
		if(size < 1) return 0;
		int n, key;
		const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
		for (n = 0; n < size; n++) {
				key = rand() % (int) (sizeof charset - 1);
				str[n] = charset[key];
		}
		str[size] = '\0';
		return size+1;
}

void toggle_bit(char *array, int index)
{
		array[index/8] ^= 1 << (index % 8);
}

char get_bit(char *array, int index)
{
		return 1 & (array[index/8] >> (index % 8));
}

void set_bit(char *array, int index, char value)
{
		if(value != 0 && value != 1) return;
		array[index/8] ^= 1 << (index % 8);
		if(get_bit(array, index) == value) return;
		toggle_bit(array, index);
}

bool get_inode_map(int index)
{
		return	get_bit(inodeMap, index);
}

void set_inode_map(int index, bool value)
{
		set_bit(inodeMap, index, value);
}

bool get_block_map(int index)
{
		return	get_bit(blockMap, index);
}

void set_block_map(int index, bool value)
{
		set_bit(blockMap, index, value);
}

int get_inode()
{
		int i;

		for(i = 0; i < MAX_INODE; i++)
		{
				if(get_inode_map(i) == 0) {
						set_inode_map(i, 1);
						superBlock.freeInodeCount--;
						return i;
				}
		}

		return -1;
}

void free_inode(int inodeNum)
{
		if(get_inode_map(inodeNum) == 1) {
				set_inode_map(inodeNum, 0);
				superBlock.freeInodeCount++;
		}
}

int get_block()
{
		int i;
		for(i = 0; i < MAX_BLOCK; i++)
		{
				if(get_block_map(i) == 0) {
						set_block_map(i, 1);
						superBlock.freeBlockCount--;
						return i;
				}
		}
		return -1;
}

void free_block(int blockNum)
{
		if(get_block_map(blockNum) == 1) {
				set_block_map(blockNum, 0);
				superBlock.freeBlockCount++;
		}
}

