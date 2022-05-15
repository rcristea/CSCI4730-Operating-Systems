// Page Table Access
PTE read_PTE (int pid, int VPN);
void write_PTE(int pid, int VPN, PTE pte);
   
/* PTE Data structure 
typedef struct {
		int PFN;
		bool valid;
		bool dirty;
} PTE;
*/


// Inverted Page Table Access 
IPTE read_IPTE (int PFN);
void write_IPTE (int PFN, IPTE ipte);

/* IPTE data structure 
typedef struct {
		int pid;
		int VPN;
} IPTE;
*/

// Swap Disk Access
void swap_in(int pid, int VPN, int PFN); // Read
void swap_out(int pid, int VPN, int PFN); // Write


