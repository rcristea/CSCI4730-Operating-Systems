#include<stdio.h>
#include<stdlib.h>
#include "clock_list.h"

//Creates a new ClockNode and returns pointer to it. 
struct ClockNode* clock_list_new_node(int x, int ref) {
		struct ClockNode* newClockNode
				= (struct ClockNode*)malloc(sizeof(struct ClockNode));
		newClockNode->data = x;
    newClockNode->ref = ref;
		newClockNode->prev = NULL;
		newClockNode->next = NULL;
		return newClockNode;
}

struct ClockNode* clock_list_remove_tail(struct ClockNode *head)
{
		if(head == NULL) return NULL;
		
		struct ClockNode *ret = head;
		
		while(ret->next != NULL) ret = ret->next; // Go To last ClockNode
		ret->prev->next = NULL;
		return head;
}

struct ClockNode* clock_list_remove_head(struct ClockNode *head)
{
		if(head == NULL) return NULL;
		
		struct ClockNode *ret = head;
		head = head->next;
		if(head != NULL) head->prev = NULL;
		
		return head;
}

struct ClockNode *clock_list_remove(struct ClockNode *head, int data)
{
		if(head == NULL) return NULL;

		struct ClockNode *ret = head;

		while(ret != NULL) {
				if(data == ret->data) {
						if(ret == head) return clock_list_remove_head(head);
						if(ret->next == NULL) return clock_list_remove_tail(head);
						if(ret->prev) ret->prev->next = ret->next;
						if(ret->next) ret->next->prev = ret->prev;
						return head;
				}
				ret = ret->next;
		}
		return head;
}

//Inserts a ClockNode at head of doubly linked list
struct ClockNode *clock_list_insert_head(struct ClockNode *head, int x, int ref) {
		struct ClockNode* newClockNode = clock_list_new_node(x, ref);
		if(head == NULL) {
				head = newClockNode;
				return head;
		}
		head->prev = newClockNode;
		newClockNode->next = head; 
		head = newClockNode;

		return head;
}

//Inserts a ClockNode at tail of Doubly linked list
struct ClockNode *clock_list_insert_tail(struct ClockNode *head, int x, int ref) {
		struct ClockNode* temp = head;
		struct ClockNode* newClockNode = clock_list_new_node(x, ref);
		if(head == NULL) {
				head = newClockNode;
				return head;
		}
		while(temp->next != NULL) temp = temp->next; // Go To last ClockNode
		temp->next = newClockNode;
		newClockNode->prev = temp;

		return head;
}

struct ClockNode *clock_list_update_ref(struct ClockNode *head, int data, int ref) {
    struct ClockNode* temp = head;
    while (temp != NULL) {
      if (temp->data == data) {
        temp->ref = ref;

        return head;
      }
      temp = temp->next;
    }

    return head;
}

// returns the reference bit if the element is found.
int clock_list_find(struct ClockNode *head, int x) {
		struct ClockNode* temp = head;
		while (temp != NULL) {
			if (temp->data == x) {
				return temp->ref;
			}

			temp = temp->next;
		}

		return -1;
}

// returns the reference of the head
int clock_list_check(struct ClockNode *head) {
  return head->ref;
}

//Prints all the elements in linked list in forward traversal order
void clock_list_print(struct ClockNode *head) {
		struct ClockNode* temp = head;
		printf("Forward: ");
		while(temp != NULL) {
				printf("[%d, %d] ",temp->data, temp->ref);
				temp = temp->next;
		}
		printf("\n");
}

//Prints all elements in linked list in reverse traversal order. 
void clock_list_reverse_print(struct ClockNode *head) {
		struct ClockNode* temp = head;
		if(temp == NULL) return; // empty list, exit
		// Going to last ClockNode
		while(temp->next != NULL) {
				temp = temp->next;
		}
		// Traversing backward using prev pointer
		printf("Reverse: ");
		while(temp != NULL) {
				printf("[%d, %d] ",temp->data, temp->ref);
				temp = temp->prev;
		}
		printf("\n");
}

