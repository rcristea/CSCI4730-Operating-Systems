#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "vm.h"

int main(int argc, char **argv)
{
		int **req;
		int i, j;

		if(argc < 4) {
				printf("usage: ./input_gen [# PAGES] [# PROCS] [# requests] \n");
				return -1;
		}

		int MAX_VPN = atoi(argv[1]);
		int NUM_PROC = atoi(argv[2]);
		int req_num = atoi(argv[3]);
		int pid, page, offset, addr;
		char type, byte;
		
		req = (int**) malloc(sizeof(int*) * NUM_PROC);
		for(i = 0; i < NUM_PROC; i++)
		{
				req[i] = (int*)malloc(sizeof(int) * MAX_VPN);
				for(j = 0; j < MAX_VPN; j++)
						req[i][j] = 0;
		}
		
		printf("# PAGES: %d, # PROCS: %d\n", MAX_VPN, NUM_PROC);
		srand(time(NULL));

		for(i = 0; i < req_num; i++)
		{
				pid = rand() % NUM_PROC;
				page = rand() % MAX_VPN;
				offset = rand() % 0xFF;
		
				int rnd = rand()%10;
				if(rnd < 5) type = 'R';
				else type = 'W';
				
				/*if(req[pid][page] == 0) {
						req[pid][page] = 1;
						type = 'W';
				} else {
						if(rand()%3 == 0) type = 'W';
						else type = 'R';
				}*/

				addr = (page << 8) + offset;
				
				if(type == 'R') printf("%d %c 0x%04x\n", pid,type, addr);
				else {
						if(rand() % 2 == 0) {
								byte = rand() % 26 + 65;
						} else {
								byte = rand() % 26 + 97;
						}
						printf("%d %c 0x%04x %c\n", pid,type, addr, byte);
				}
		}

		return 0;
}

