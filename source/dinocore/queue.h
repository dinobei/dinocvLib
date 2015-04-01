#ifndef __QUEUE_H__
#define __QUEUE_H__
#include "../common/def_os_selector_header.h"
#include "linkedlist.h"

typedef LIST_D QUEUE_D;

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	QUEUE_D* queue_malloc();

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	bool_d queue_put(QUEUE_D *qu, void *data);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void *queue_get(QUEUE_D *qu);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void queue_free(QUEUE_D *qu);

#ifdef __cplusplus
}
#endif

#endif
