#include "../common/def_os_selector_export.h"
#include "linkedlist.h"

// NODE
NODE_D* dinocv_node_malloc()
{
	NODE_D *retv = (NODE_D*)malloc(sizeof(NODE_D));
	memset(retv, 0, sizeof(NODE_D));
	return retv;
}

// LIST
LIST_D* dinocv_list_malloc(){
	LIST_D *retv = (LIST_D *)malloc(sizeof(LIST_D));
	memset(retv, 0, sizeof(LIST_D));
	return retv;
}

/*
  node 안의 동적할당이 어떻게 되어있는지 모르므로 사용하면 안됨.
  만약 사용하려면, 메모리 해지 기능을 하는 함수를 만들어야함 
  */
void dinocv_list_free(LIST_D* list){
	NODE_D *node, *del_node;
	
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

bool_d dinocv_list_add_head(LIST_D* list, void *data){
	NODE_D *new_node, *temp_node;

	if(!list || !data) return d_false;

	// data input
	new_node = dinocv_node_malloc();
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

bool_d dinocv_list_add_tail(LIST_D* list, void *data){
	NODE_D *new_tail_node, *old_tail_node;

	if(!list || !data) return d_false;

	// data input
	new_tail_node = dinocv_node_malloc();
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

NODE_D *dinocv_list_del_head(LIST_D* list){
	NODE_D *del_node;

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

NODE_D *dinocv_list_del_tail(LIST_D* list){
	int i;
	NODE_D *del_node=NULL, *parent=NULL;

	if(!list) return NULL;
	if(!list->head) return NULL;

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
	return del_node;
}

void *dinocv_list_del_idx_data(LIST_D *list, int idx){
	NODE_D *cr_node = list->head;
	NODE_D *bf_node=NULL;
	void *ret_buf;

	if(!list) return NULL;
	if(idx >= list->cnt) return NULL;
	if(!list->cnt) return NULL;

	if(list->cnt==1){
		memset(list, 0, sizeof(LIST_D));
		ret_buf = cr_node->data;
		free(cr_node);
		return ret_buf;
	}

	if(idx == 0) {
		cr_node = dinocv_list_del_head(list);
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

void *dinocv_list_get_head_data(LIST_D* list){
	if(!list) return NULL;
	if(!list->cnt) return NULL;
	return list->head->data;
}

void *dinocv_list_get_tail_data(LIST_D* list){
	if(!list) return NULL;
	if(!list->cnt) return NULL;
	return list->tail->data;
}

void *dinocv_list_get_idx_data(LIST_D *list, int idx){
	NODE_D *cr_node = list->head;

	if(!list) return NULL;
	if(!list->cnt) return NULL;
	if(idx >= list->cnt) return NULL;

	while(idx--) cr_node = cr_node->next;
	return cr_node->data;
}
