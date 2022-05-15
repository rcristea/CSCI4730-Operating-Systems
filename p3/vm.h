#include <stdbool.h>

#define NDEBUG

#ifdef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) fprintf(stderr, "DEBUG(%s:%d) " M, __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#define ZERO 0
#define FIFO 1
#define LRU 2
#define CLOCK 3

typedef struct {
		int pid;
		int VPN;
} IPTE;

typedef struct {
		int PFN;
		bool valid;
		bool dirty;
} PTE;

typedef struct {
		PTE *entry;  //PTE entry[NUM_PAGE];
} PT;

int pagefault_handler(int pid, int pageNo, char type);

typedef struct {
		int hitCount;
		int missCount;
		int swapRead;
		int swapWrite;
} STATS;

extern int MAX_PFN;
extern int MAX_VPN;
extern int MAX_PID;

extern int replacementPolicy;
extern STATS stats;

int page_replacement();
int MMU(int pid, int addr, char type, bool *hit);
void pt_print_stats();
void init_pagetable();
void init_swap_disk();
void init_freeframe();
void swap_print_stats();

