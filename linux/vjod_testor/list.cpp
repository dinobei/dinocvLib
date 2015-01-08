#include "list.h"

// NODE
NODE_D* soc_node_malloc()
{
	NODE_D *retv = (NODE_D*)malloc(sizeof(NODE_D));
	memset(retv, 0, sizeof(NODE_D));
	return retv;
}

// LIST
LIST_D* soc_list_malloc(){
	LIST_D *retv;
	retv = (LIST_D *)malloc(sizeof(LIST_D));
	memset(retv, 0, sizeof(LIST_D));
	return retv;
}

void soc_list_free(LIST_D* list){
	NODE_D *node, *del_node;
	
	if( !list ) return;
	if(list->cnt > 0)
	{
		node = list->head;
		do{
			del_node = node;
			node = node->next;
			free(del_node);
		}while(node);
	}
	free(list);
}

BOOL soc_list_add_head(LIST_D* list, void *data){
	NODE_D *new_node, *temp_node;

	if(!list || !data) return FALSE;

	// input data
	new_node = soc_node_malloc();
	new_node->data = data;

	// add to head
	if(!list->head)
	{
		list->head = new_node;
		list->tail = new_node;
	}
	else{
		temp_node = list->head;
		list->head = new_node;
		new_node->next = temp_node;
	}
	list->cnt++;
	return TRUE;
}

BOOL soc_list_add_tail(LIST_D* list, void *data){
	NODE_D *new_tail_node;

	if(!list || !data) return FALSE;

	// input data
	new_tail_node = soc_node_malloc();
	new_tail_node->data = data;
	
	// add to tail
	if(!list->tail)
	{
		list->head = new_tail_node;
		list->tail = new_tail_node;
	}
	else
	{
		list->tail->next = new_tail_node;
		list->tail = new_tail_node;
	}
	list->cnt++;
	return TRUE;
}

void *soc_list_del_head(LIST_D* list){
	NODE_D *del_node;
	void *ret_buf;

	if(!list) return NULL;
	if(!list->cnt) return NULL;

	del_node = list->head;
	list->head = list->head->next;
	
	// if cnt > 1, then tail maintain it's value.
	if(list->cnt == 1)
		list->tail = NULL;

	list->cnt--;
	
	ret_buf = del_node->data;
	free(del_node);
	return ret_buf;
}

void *soc_list_del_tail(LIST_D* list){
	int i;
	NODE_D *del_node, *parent;
	void *ret_buf;

	if(!list) return NULL;
	if(!list->cnt) return NULL;
	
	del_node = list->head;
	if(list->cnt == 1){
		memset(list, 0, sizeof(LIST_D));
		return del_node;
	}

	for(i = list->cnt-1 ; i-- ; ){
		parent = del_node;
		del_node = del_node->next;
	}

	parent->next = NULL;
	list->tail = parent;
	list->cnt--;
	
	ret_buf = del_node->data;
	free(del_node);
	return ret_buf;
}

void *soc_list_del_idx_data(LIST_D *list, int idx){
	NODE_D *cr_node = list->head;
	NODE_D *bf_node;
	void *ret_buf;

	if(!list) return NULL;
	if(!list->cnt) return NULL;
	if(idx >= list->cnt) return NULL;
	

	// CNT == 1
	if(list->cnt==1)
	{
		memset(list, 0, sizeof(LIST_D));
		ret_buf = cr_node->data;
		free(cr_node);
		return ret_buf;
	}

	// CNT >= 2
	if(idx == 0)
	{
		ret_buf = soc_list_del_head(list);
		return ret_buf;
	}

	while(idx--){
		bf_node = cr_node;
		cr_node = cr_node->next;
	};
	bf_node->next = cr_node->next;
	list->cnt--;
	ret_buf = cr_node->data;
	free(cr_node);
	return ret_buf;
}

void *soc_list_get_head_data(LIST_D* list){
	if(!list) return NULL;
	if(!list->cnt) return NULL;
	return list->head->data;
}

void *soc_list_get_tail_data(LIST_D* list){
	if(!list) return NULL;
	if(!list->cnt) return NULL;
	return list->tail->data;
}

void *soc_list_get_idx_data(LIST_D *list, int idx){
	NODE_D *cr_node = list->head;

	if(!list) return NULL;
	if(!list->cnt) return NULL;
	if(idx >= list->cnt) return NULL;

	while(idx--) cr_node = cr_node->next;
	return cr_node->data;
}