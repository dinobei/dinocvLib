#ifndef __QUEUE_H__
#define __QUEUE_H__
#include "../common/def_os_selector_header.h"
#include "linkedlist.h"

typedef LIST_T QUEUE_T;

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	QUEUE_T* queue_malloc();

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	bool_d queue_put(QUEUE_T *qu, void *data);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void *queue_get(QUEUE_T *qu);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void queue_free(QUEUE_T *qu);

#ifdef __cplusplus
}
#endif

#endif
