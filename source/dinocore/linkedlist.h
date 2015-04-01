#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__
#include "../common/def_os_selector_header.h"
#include <stdlib.h>
#include <string.h>

typedef int bool_d;
#define d_true 1
#define d_false 0

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


#ifdef __cplusplus
extern "C"
{
#endif

// NODE
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	NODE_D*		dinocv_node_malloc();

// LIST
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	LIST_D*		dinocv_list_malloc();

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void		dinocv_list_free(LIST_D *list);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	bool_d		dinocv_list_add_head(LIST_D *list, void *data);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	bool_d		dinocv_list_add_tail(LIST_D *list, void *data);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	NODE_D*		dinocv_list_del_head(LIST_D *list);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	NODE_D*		dinocv_list_del_tail(LIST_D *list);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void *		dinocv_list_del_idx_data(LIST_D *list, int idx);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void *		dinocv_list_get_head_data(LIST_D *list);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void *		dinocv_list_get_tail_data(LIST_D *list);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void *		dinocv_list_get_idx_data(LIST_D *list, int idx);

#ifdef __cplusplus
}
#endif

#endif
