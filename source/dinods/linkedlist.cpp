#include "../common/def_os_selector_export.h"
#include "linkedlist.h"

// NODE
NODE_T* node_malloc()
{
	NODE_T *retv = (NODE_T*)malloc(sizeof(NODE_T));
	memset(retv, 0, sizeof(NODE_T));
	return retv;
}

// LIST
LIST_T* list_malloc(){
	LIST_T *retv = (LIST_T *)malloc(sizeof(LIST_T));
	memset(retv, 0, sizeof(LIST_T));
	return retv;
}

/*
  node 안의 동적할당이 어떻게 되어있는지 모르므로 사용하면 안됨.
  만약 사용하려면, 메모리 해지 기능을 하는 함수를 만들어야함 
  */
void list_free(LIST_T* list){
	NODE_T *node, *del_node;
	
	if(!list) return;
	if(list->cnt)
	{
		node = list->head;
		do
		{
			del_node = node;
			node = node->next;

			// 이 부분에 data를 free하는 코드를 넣어야함
			free(del_node);
		}while(node);
	}
	free(list);
}

bool_d list_add_head(LIST_T* list, void *data){
	NODE_T *new_node, *temp_node;

	if(!list || !data) return d_false;

	// data input
	new_node = node_malloc();
	new_node->data = data;

	if(!list->cnt){
		list->head = new_node;
		list->tail = new_node;
	}
	else{
		temp_node = list->head;
		list->head = new_node;
		new_node->next = temp_node;
	}

	list->cnt++;
	return d_true;
}

bool_d list_add_tail(LIST_T* list, void *data){
	NODE_T *new_tail_node, *old_tail_node;

	if(!list || !data) return d_false;

	// data input
	new_tail_node = node_malloc();
	new_tail_node->data = data;

	// tail reset
	old_tail_node = list->tail;
	list->tail = new_tail_node;
	
	if(!list->head)
		list->head = new_tail_node;
	else
		old_tail_node->next = new_tail_node;

	list->cnt++;
	return d_true;
}

NODE_T *list_del_head(LIST_T* list){
	NODE_T *del_node;

	if(!list) return NULL;
	if(!list->cnt) return NULL;

	// if cnt > 1, then tail maintain its value.
	if(list->cnt == 1)
		list->tail = NULL;

	del_node = list->head;
	list->head = list->head->next;
	
	list->cnt--;
	return del_node;
}

NODE_T *list_del_tail(LIST_T* list){
	int i;
	NODE_T *del_node=NULL, *parent=NULL;

	if(!list) return NULL;
	if(!list->head) return NULL;

	del_node = list->head;
	if(list->cnt == 1){
		memset(list, 0, sizeof(LIST_T));
		return del_node;
	}

	for(i = list->cnt-1 ; i-- ; ){
		parent = del_node;
		del_node = del_node->next;
	}

	parent->next = NULL;
	list->tail = parent;
	list->cnt--;
	return del_node;
}

void *list_del_idx_data(LIST_T *list, int idx){
	NODE_T *cr_node = list->head;
	NODE_T *bf_node=NULL;
	void *ret_buf;

	if(!list) return NULL;
	if(idx >= list->cnt) return NULL;
	if(!list->cnt) return NULL;

	if(list->cnt==1){
		memset(list, 0, sizeof(LIST_T));
		ret_buf = cr_node->data;
		free(cr_node);
		return ret_buf;
	}

	if(idx == 0) {
		cr_node = list_del_head(list);
		ret_buf = cr_node->data;
		free(cr_node);
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

void *list_get_head_data(LIST_T* list){
	if(!list) return NULL;
	if(!list->cnt) return NULL;
	return list->head->data;
}

void *list_get_tail_data(LIST_T* list){
	if(!list) return NULL;
	if(!list->cnt) return NULL;
	return list->tail->data;
}

void *list_get_idx_data(LIST_T *list, int idx){
	NODE_T *cr_node = list->head;

	if(!list) return NULL;
	if(!list->cnt) return NULL;
	if(idx >= list->cnt) return NULL;

	while(idx--) cr_node = cr_node->next;
	return cr_node->data;
}
