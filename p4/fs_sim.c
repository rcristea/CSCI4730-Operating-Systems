#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "fs.h"
#include "disk.h"

bool command(char *comm, char *comm2)
{
		if(strlen(comm) == strlen(comm2) && strncmp(comm, comm2, strlen(comm)) == 0) return true;
		return false;
}

int main(int argc, char **argv)
{
		char input[64+16+16+16];
		char comm[64], arg1[16], arg2[16];

		srand(10);

		if(argc < 2) {
				fprintf(stderr, "usage: ./fs disk_name\n");
				return -1;
		}
		srand(10);
		
		printf("sizeof inode: %d, sizeof superblock: %d, sizeof Dentry: %d\n", sizeof(Inode), sizeof(SuperBlock), sizeof(Dentry));
		fs_mount(argv[1]);
		printf("%% ");
		while(fgets(input, 256, stdin))
		{
				printf("command: %s", input);
				bzero(comm,64); bzero(arg1,16); bzero(arg2,16);
				int numArg = sscanf(input, "%s %s %s", comm, arg1, arg2);
				if(command(comm, "quit")) break;
				else if(command(comm, "exit")) break;
				else execute_command(comm, arg1, arg2, numArg - 1);

				printf("\n%% ");
		}

		fs_umount(argv[1]);
}

