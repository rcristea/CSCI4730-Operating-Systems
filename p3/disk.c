#include <stdio.h>
#include <stdlib.h>
#include "vm.h"

extern char *PHYSICAL_MEM;
char **SWAP_DISK;
extern STATS stats;

void init_swap_disk()
{
		int i,j;

		SWAP_DISK = (char**) malloc(sizeof(char*) * MAX_PID);
		if(SWAP_DISK == NULL) {
				fprintf(stderr, "swap disk init failed. Please execute the program again.\n");
				exit(0);
		}

		for(i = 0; i < MAX_PID; i++)
		{
				SWAP_DISK[i] = (char*) malloc(sizeof(char) * MAX_VPN);
				for(j = 0; j < MAX_VPN; j++)
						SWAP_DISK[i][j] = 'A';
		}
}

void swap_in(int pid, int VPN, int PFN)
{
		stats.swapRead++;
		PHYSICAL_MEM[PFN] = SWAP_DISK[pid][VPN];
		debug("swap in (disk -> mem): pid %d, page %d, frame %d, byte %c\n", pid, VPN, PFN, SWAP_DISK[pid][VPN]);
}

void swap_out(int pid, int VPN, int PFN)
{
		stats.swapWrite++;
		SWAP_DISK[pid][VPN]= PHYSICAL_MEM[PFN];
		debug("swap in (mem -> disk): pid %d, page %d, frame %d, byte %c\n", pid, VPN, PFN, PHYSICAL_MEM[PFN]);
}


