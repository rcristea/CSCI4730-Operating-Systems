#include <sys/time.h>
#include "disk.h"

#define MAX_BLOCK 4096
#define MAX_INODE 512
#define MAX_FILE_NAME 20
#define MAGIC_NUMBER 0x1234FFFF
#define MAX_DIR_ENTRY BLOCK_SIZE / sizeof(DirectoryEntry)

typedef enum {file, directory} TYPE;

typedef struct {
		int magicNumber;
		int freeBlockCount; // # of available data blocks
		int freeInodeCount; // # of available inodes
		int rootDirInode;   // root directory
		char padding[496];  // padding bytes to make superblock to 512 bytes
} SuperBlock;

typedef struct {
		TYPE type;  // file or directory
		int size;			// file size
		int blockCount;  // # of blocks used
		int directBlock[15];
		int indirectBlock;
		int linkCount; // for hardlink
		char padding[48]; // Padding bytes to make the inode size to 128 byte
} Inode; // 128 byte

typedef struct {
		char name[MAX_FILE_NAME]; // max length of file/directory name is 20 bytes.
		int inode;
} DirectoryEntry;

typedef struct {
		int numEntry;  // number of files and sub-directories
		DirectoryEntry dentry[MAX_DIR_ENTRY]; // each entry has "name" and "inode"
		char padding[4]; // padding bytes to make it to 512 byte
} Dentry;

extern char inodeMap[MAX_INODE / 8];
extern char blockMap[MAX_BLOCK / 8];
extern SuperBlock superBlock;

int fs_mount(char *name);
int fs_umount(char *name);
int execute_command(char *comm, char *arg1, char *arg2, int numArg);
