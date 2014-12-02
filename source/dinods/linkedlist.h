#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__
#include "../common/def_os_selector_header.h"
#include <stdlib.h>
#include <string.h>

typedef int bool_d;
#define d_true 1
#define d_false 0

typedef struct NODE_SOURCE NODE_T;
struct NODE_SOURCE
{
	void *data;
	NODE_T* next;
};

typedef struct LIST_SOURCE LIST_T;
struct LIST_SOURCE
{
	NODE_T* head;
	NODE_T* tail;
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
	NODE_T*		node_malloc();

// LIST
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	LIST_T*		list_malloc();

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void		list_free(LIST_T *list);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	bool_d		list_add_head(LIST_T *list, void *data);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	bool_d		list_add_tail(LIST_T *list, void *data);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	NODE_T*		list_del_head(LIST_T *list);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	NODE_T*		list_del_tail(LIST_T *list);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void *		list_del_idx_data(LIST_T *list, int idx);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void *		list_get_head_data(LIST_T *list);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void *		list_get_tail_data(LIST_T *list);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void *		list_get_idx_data(LIST_T *list, int idx);

#ifdef __cplusplus
}
#endif

#endif
