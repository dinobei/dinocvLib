#ifndef __DS_SORT_H__
#define __DS_SORT_H__
#include "../common/def_os_selector_header.h"

#ifdef __cplusplus
extern "C"
{
#endif

//extern "C" DINOBEI_DLLTYPE
//	void ds_q_sort(int *numbers, int left, int right);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_sort_insert(int *d, int n);

#ifdef __cplusplus
}
#endif

#endif
