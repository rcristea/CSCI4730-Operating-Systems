struct ClockNode  {
		int data;
    int ref;
		struct ClockNode* next;
		struct ClockNode* prev;
};

struct ClockNode* clock_list_new_node(int data, int ref);
struct ClockNode* clock_list_insert_head(struct ClockNode *head,int data, int ref);
struct ClockNode* clock_list_insert_tail(struct ClockNode *head,int data, int ref);
struct ClockNode* clock_list_remove_head(struct ClockNode *head);
struct ClockNode* clock_list_remove_tail(struct ClockNode *head);
struct ClockNode* clock_list_remove(struct ClockNode *head, int data);
struct ClockNode* clock_list_update_ref(struct ClockNode *head, int data, int ref);
int clock_list_check(struct ClockNode *head);
int clock_list_find(struct ClockNode *head, int x);
void clock_list_print(struct ClockNode *head);
void clock_list_reverse_print(struct ClockNode *head);
