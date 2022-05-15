#include <stdio.h>
#include <stdlib.h>
#include "vm.h"
#include "list.h"

PT *pageTable;
IPTE *invertedPageTable;

PTE read_PTE(int pid, int VPN)
{
		return pageTable[pid].entry[VPN];
}

void write_PTE(int pid, int VPN, PTE pte)
{
		pageTable[pid].entry[VPN].PFN = pte.PFN;
		pageTable[pid].entry[VPN].valid = pte.valid;
		pageTable[pid].entry[VPN].dirty = pte.dirty;
}

IPTE read_IPTE(int PFN)
{
		return invertedPageTable[PFN];
}

void write_IPTE(int PFN, IPTE ipte)
{
		invertedPageTable[PFN].pid = ipte.pid;
		invertedPageTable[PFN].VPN = ipte.VPN;
}

int nextFreeFrame = 0;
int get_freeframe()
{
		if(nextFreeFrame >= MAX_PFN) return -1;
		
		return nextFreeFrame++;
}

void init_pagetable()
{
		int i,j;

		pageTable = (PT*) malloc(sizeof(PT) * MAX_PID);
		invertedPageTable = (IPTE*) malloc(sizeof(IPTE*) * MAX_PFN);
		stats.hitCount = 0;
		stats.missCount = 0;

		for(i = 0; i < MAX_PID; i++) {
				pageTable[i].entry = (PTE*) malloc(sizeof(PTE) * MAX_VPN);
				for(j = 0; j < MAX_VPN; j++) {
						pageTable[i].entry[j].valid = false;
				}
		}
}

