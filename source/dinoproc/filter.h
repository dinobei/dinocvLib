#ifndef __FILTER_H__
#define __FILTER_H__
#include "../common/def_os_selector_header.h"
#include "../dinocore/imgcore.h"
#include "../dinocore/ds_sort.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_filter_mean(IMAGE_D *image);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_filter_weight_mean(IMAGE_D *image);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_filter_median(IMAGE_D *img);

#ifdef __cplusplus
}
#endif

#endif
