#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include <string.h>
#include <stdlib.h>

typedef int BOOL;
#define TRUE 1
#define FALSE 0

typedef struct NODE_D NODE_D;
struct NODE_D
{
	void *data;
	NODE_D* next;
};

typedef struct LIST_D LIST_D;
struct LIST_D
{
	NODE_D* head;
	NODE_D* tail;
	int cnt;
};

// NODE
NODE_D*		soc_node_malloc();

// LIST
LIST_D*		soc_list_malloc();
void		soc_list_free(LIST_D *list);
BOOL		soc_list_add_head(LIST_D *list, void *data); // Add
BOOL		soc_list_add_tail(LIST_D *list, void *data);
void*		soc_list_del_head(LIST_D *list); // Del
void*		soc_list_del_tail(LIST_D *list);
void *		soc_list_del_idx_data(LIST_D *list, int idx);
void *		soc_list_get_head_data(LIST_D *list); // Get
void *		soc_list_get_tail_data(LIST_D *list);
void *		soc_list_get_idx_data(LIST_D *list, int idx);

#endif