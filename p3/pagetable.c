#include <stdio.h>
#include <stdlib.h>
#include "vm.h"
#include "API.h"
#include "clock_list.h"
#include "list.h"

int i = -1;
struct Node *head = NULL;
struct ClockNode *clockHead = NULL;

int fifo() {
	i++;
	return i % MAX_PFN;
}

int lru() {
	return head->data;
}

int clock() {
	/*
	
		1 2 3 4 1 2 5 1 2 3 4 5
		0 1 2 3 0 1 0 1 2 3 0 1

	*/

	while (clockHead->ref == 1) {
		int p = clockHead->data;
		// printf("list_before "); clock_list_print(clockHead);
		clockHead = clock_list_remove_head(clockHead);
		// printf("remove_head "); clock_list_print(clockHead);
		clockHead = clock_list_insert_tail(clockHead, p, 0);
		// printf("insert_tail "); clock_list_print(clockHead);
	}

	return clockHead->data;
//   while (clockHead->ref == 1) {
//     struct ClockNode *temp = clockHead;
//     clockHead = clock_list_remove_head(clockHead);
//     clockHead = clock_list_insert_tail(clockHead, temp->data, 0);
//   } 

//   int data = clockHead->data;
//   clockHead = clock_list_remove_head(clockHead);

//   return data;
//   for (i = 0; i < MAX_PFN; i++) {
// 	  if (clock_flag[i] == 1) {
// 		  clock_flag[i] == 0;
// 	  } else {
// 		  clock_index = i;
// 		  clock_flag[i] = 1;
// 		  return clock_index;
// 	  }
//   }
//   return clock_index;
}

/*========================================================================*/

int find_replacement() {
	int PFN;
	if(replacementPolicy == ZERO)  PFN = 0;
	else if(replacementPolicy == FIFO)  PFN = fifo();
	else if(replacementPolicy == LRU) PFN = lru();
	else if(replacementPolicy == CLOCK) PFN = clock();

	return PFN;
}

int pagefault_handler(int pid, int VPN, char type) {
	int PFN;

	// find a free PFN.
	PFN = get_freeframe();

	// no free frame available. find a victim using page replacement. ;
	if(PFN < 0) {
		PFN = find_replacement();

		if (replacementPolicy == LRU) {
      		head = list_remove_head(head);
		} else if (replacementPolicy == CLOCK) {
			// printf("list_before "); clock_list_print(clockHead);
			clockHead = clock_list_remove_head(clockHead);
			// printf("remvoe_head "); clock_list_print(clockHead);
    	}

		IPTE ipte = read_IPTE(PFN);
		PTE pte = read_PTE(ipte.pid, ipte.VPN);
		swap_out(ipte.pid, ipte.VPN, pte.PFN);
		// if (pte.dirty) {
		// 	swap_out(ipte.pid, VPN, PFN);
		// 	pte.dirty = false;
		// }

		pte.valid = false;
		write_PTE(ipte.pid, ipte.VPN, pte);
	}
  
	if (replacementPolicy == LRU) {
		head = list_insert_tail(head, PFN);
	} else if (replacementPolicy == CLOCK) {
		// printf("list_before "); clock_list_print(clockHead);
    	clockHead = clock_list_insert_tail(clockHead, PFN, 0);
		// printf("insert_tail "); clock_list_print(clockHead);
	}

	IPTE ipte = read_IPTE(PFN);
	PTE pte = read_PTE(ipte.pid, ipte.VPN);

	if (type == 'W') {
		pte.dirty = true;
	} else if (type == 'R') {
		pte.dirty = false;
	}

	swap_in(pid, VPN, PFN);
	ipte.pid = pid;
	ipte.VPN = VPN;
	write_IPTE(PFN, ipte);
	pte.valid = true;
	pte.PFN = PFN;
	write_PTE(pid, VPN, pte);
	
	// TODO DON'T DELETE THIS LINE UNTIL DONE, IT'LL BE ANNOYING TO REWRITE
	// printf("pte.PFN[%d] pte.valid[%d] pte.dirty[%d]\n", pte.PFN, pte.valid, pte.dirty);
	
	/* ---- */
	return PFN;
}

int is_page_hit(int pid, int VPN, char type) {
	/* Read page table entry for (pid, VPN) */
	PTE pte = read_PTE(pid, VPN);

	/* if PTE is valid, it is a page hit. Return physical frame number (PFN) */
	if(pte.valid) {
		/* Mark the page dirty, if it is a write request */
		if(type == 'W') {
			pte.dirty = true;
			write_PTE(pid, VPN, pte);
		}

		/* Need to take care of a page replacement data structure (LRU, CLOCK) for the page hit*/
		if (replacementPolicy == LRU) {
			head = list_remove(head, pte.PFN);
			head = list_insert_tail(head, pte.PFN);
		} else if (replacementPolicy == CLOCK) {
			// printf("list_before "); clock_list_print(clockHead);
			clockHead = clock_list_update_ref(clockHead, pte.PFN, 1);
			// printf("update_ref  "); clock_list_print(clockHead);
		}

		return pte.PFN;
	}
	/* PageFault, if the PTE is invalid. Return -1 */
	return -1;
}

int MMU(int pid, int VPN, char type, bool *hit) {
	int PFN;

	// hit
	PFN = is_page_hit(pid, VPN, type);
	if(PFN >= 0) {
		stats.hitCount++;
		*hit = true;
		return PFN; 
	}

	stats.missCount++;
	*hit = false;
			
	// miss -> pagefault
	PFN = pagefault_handler(pid, VPN, type);


	return PFN;
}

