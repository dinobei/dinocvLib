#ifndef __DP_H__
#define __DP_H__
#include "../common/def_os_selector_header.h"
#include "../dinocore/imgcore.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_douglas_peucker(double tolerance, POINT_D2 *Vertex, int istart, int iend, int *mark); // DP Algorithm

#ifdef __cplusplus
}
#endif

#endif
